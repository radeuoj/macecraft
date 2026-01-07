use std::sync::Arc;
use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;
use winit::event::WindowEvent;
use winit::window::{Window, WindowId};
use crate::block::Block;
use crate::camera::Camera;
use crate::imgui::ImGuiState;
use crate::texture::{DepthTexture, Texture};

#[repr(C)]
#[derive(Copy, Clone, Default, Pod, Zeroable)]
struct Vertex {
    position: [f32; 3],
    tex_coords: [f32; 2],
}

impl Vertex {
    fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 2] = wgpu::vertex_attr_array![
            0 => Float32x3,
            1 => Float32x2,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<Vertex>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

const VERTICES: [Vertex; 8] = [
    Vertex { position: [-0.5, 0.5, -1.0], tex_coords: [0.0, 0.0] },
    Vertex { position: [-0.5, -0.5, -1.0], tex_coords: [0.0, 1.0] },
    Vertex { position: [0.5, -0.5, -1.0], tex_coords: [1.0, 1.0] },
    Vertex { position: [0.5, 0.5, -1.0], tex_coords: [1.0, 0.0] },

    Vertex { position: [-0.5, 0.5, -2.0], tex_coords: [0.0, 0.0] },
    Vertex { position: [-0.5, -0.5, -2.0], tex_coords: [0.0, 1.0] },
    Vertex { position: [0.5, -0.5, -2.0], tex_coords: [1.0, 1.0] },
    Vertex { position: [0.5, 0.5, -2.0], tex_coords: [1.0, 0.0] },
];

const INDICES: [u16; 12] = [
    0, 1, 2,
    2, 3, 0,

    4, 5, 6,
    6, 7, 4,
];

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct CameraUniform {
    view_proj: [[f32; 4]; 4],
}

impl CameraUniform {
    fn new() -> Self {
        Self {
            view_proj: glam::Mat4::IDENTITY.to_cols_array_2d(),
        }
    }

    fn update_view_proj(&mut self, camera: &Camera) {
        self.view_proj = camera.build_view_proj_matrix().to_cols_array_2d();
    }
}

pub struct Renderer {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: wgpu::Surface<'static>,
    surface_format: wgpu::TextureFormat,
    render_pipeline: wgpu::RenderPipeline,

    vertex_buffer: wgpu::Buffer,
    index_buffer: wgpu::Buffer,

    diffuse_bind_group: wgpu::BindGroup,

    camera_uniform: CameraUniform,
    camera_buffer: wgpu::Buffer,
    camera_bind_group: wgpu::BindGroup,

    depth_texture: DepthTexture,

    backend: wgpu::Backend,

    imgui: ImGuiState,
    imgui_fn: Option<Box<dyn FnMut(&mut imgui::Ui)>>,

    blocks: Vec<wgpu::Buffer>,
}

impl Renderer {
    pub async fn new(window: Arc<Window>, camera: &Camera) -> Self {
        let instance = wgpu::Instance::new(&wgpu::InstanceDescriptor::from_env_or_default());

        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions::default())
            .await
            .unwrap();

        let (device, queue) = adapter
            .request_device(&wgpu::DeviceDescriptor::default())
            .await
            .unwrap();

        let size = window.inner_size();

        let surface = instance.create_surface(window.clone()).unwrap();
        let surface_caps = surface.get_capabilities(&adapter);
        let surface_format = surface_caps.formats[0];
        surface.configure(&device, &get_surface_config(surface_format, size));

        let shader = device
            .create_shader_module(wgpu::include_wgsl!("shader.wgsl"));

        let diffuse_bytes = include_bytes!("textures/dirt.png");
        let diffuse_texture = Texture::from_bytes(
            &device,
            &queue,
            diffuse_bytes,
            "dirt",
        ).unwrap();

        let texture_bind_group_layout = device
            .create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: Some("Texture Bind group layout"),
                entries: &[
                    wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Texture {
                            sample_type: wgpu::TextureSampleType::Float { filterable: true },
                            view_dimension: wgpu::TextureViewDimension::D2,
                            multisampled: false,
                        },
                        count: None,
                    },
                    wgpu::BindGroupLayoutEntry {
                        binding: 1,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Sampler(wgpu::SamplerBindingType::Filtering),
                        count: None,
                    },
                ],
            });

        let diffuse_bind_group = device
            .create_bind_group(&wgpu::BindGroupDescriptor {
                label: Some("Diffuse bind group"),
                layout: &texture_bind_group_layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: wgpu::BindingResource::TextureView(&diffuse_texture.view),
                    },
                    wgpu::BindGroupEntry {
                        binding: 1,
                        resource: wgpu::BindingResource::Sampler(&diffuse_texture.sampler),
                    },
                ],
            });

        let mut camera_uniform = CameraUniform::new();
        camera_uniform.update_view_proj(camera);

        let camera_buffer = device
            .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some("Camera buffer"),
                contents: bytemuck::cast_slice(&[camera_uniform]),
                usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            });

        let camera_bind_group_layout = device
            .create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: Some("Camera Bind group layout"),
                entries: &[
                    wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::VERTEX,
                        ty: wgpu::BindingType::Buffer {
                            ty: wgpu::BufferBindingType::Uniform,
                            has_dynamic_offset: false,
                            min_binding_size: None,
                        },
                        count: None,
                    },
                ],
            });

        let camera_bind_group = device
            .create_bind_group(&wgpu::BindGroupDescriptor {
                label: Some("Camera bind group"),
                layout: &camera_bind_group_layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: camera_buffer.as_entire_binding(),
                    },
                ],
            });

        let depth_texture = DepthTexture::new(&device, size.into(), "Depth texture");

        let render_pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Render pipeline layout"),
                bind_group_layouts: &[
                    &texture_bind_group_layout,
                    &camera_bind_group_layout,
                ],
                immediate_size: 0,
            });

        let render_pipeline = device
            .create_render_pipeline(&wgpu::RenderPipelineDescriptor {
                label: Some("Render pipeline"),
                layout: Some(&render_pipeline_layout),
                vertex: wgpu::VertexState {
                    module: &shader,
                    entry_point: Some("vs_main"),
                    compilation_options: Default::default(),
                    buffers: &[Vertex::layout()],
                },
                fragment: Some(wgpu::FragmentState {
                    module: &shader,
                    entry_point: Some("fs_main"),
                    compilation_options: Default::default(),
                    targets: &[Some(wgpu::ColorTargetState {
                        format: surface_format,
                        blend: Some(wgpu::BlendState::REPLACE),
                        write_mask: wgpu::ColorWrites::ALL,
                    })],
                }),
                primitive: wgpu::PrimitiveState {
                    topology: wgpu::PrimitiveTopology::TriangleList,
                    strip_index_format: None,
                    front_face: wgpu::FrontFace::Ccw,
                    cull_mode: Some(wgpu::Face::Back),
                    unclipped_depth: false,
                    polygon_mode: wgpu::PolygonMode::Fill,
                    conservative: false,
                },
                depth_stencil: Some(wgpu::DepthStencilState {
                    format: DepthTexture::DEPTH_FORMAT,
                    depth_write_enabled: true,
                    depth_compare: wgpu::CompareFunction::Less,
                    stencil: wgpu::StencilState::default(),
                    bias: wgpu::DepthBiasState::default(),
                }),
                multisample: wgpu::MultisampleState {
                    count: 1,
                    mask: !0,
                    alpha_to_coverage_enabled: false,
                },
                multiview_mask: None,
                cache: None,
            });

        let vertex_buffer = device
            .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some("Vertex buffer"),
                contents: bytemuck::cast_slice(&VERTICES),
                usage: wgpu::BufferUsages::VERTEX,
            });

        let index_buffer = device
            .create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some("Index buffer"),
                contents: bytemuck::cast_slice(&INDICES),
                usage: wgpu::BufferUsages::INDEX,
            });

        let imgui = ImGuiState::new(
            &device,
            &queue,
            surface_format,
            &window,
        );

        Self {
            device,
            queue,
            surface,
            surface_format,
            render_pipeline,
            vertex_buffer,
            index_buffer,
            diffuse_bind_group,
            camera_uniform,
            camera_buffer,
            camera_bind_group,
            depth_texture,
            backend: adapter.get_info().backend,
            imgui,
            imgui_fn: None,
            blocks: vec![],
        }
    }

    pub fn get_backend(&self) -> wgpu::Backend {
        self.backend
    }

    pub fn resize(&mut self, size: winit::dpi::PhysicalSize<u32>) {
        self.surface.configure(&self.device, &get_surface_config(self.surface_format, size));
        self.depth_texture = DepthTexture::new(&self.device, size.into(), "Depth texture");
    }

    pub fn update_camera(&mut self, camera: &Camera) {
        self.camera_uniform.view_proj = camera.build_view_proj_matrix().to_cols_array_2d();
        self.queue.write_buffer(&self.camera_buffer, 0, bytemuck::cast_slice(&[self.camera_uniform]));
    }

    pub fn update_delta_time(&mut self, delta_time: f32) {
        self.imgui.update_delta_time(delta_time);
    }

    pub fn render_imgui(&mut self, imgui_fn: impl FnMut(&mut imgui::Ui) + 'static) {
        self.imgui_fn = Some(Box::new(imgui_fn));
    }

    pub fn imgui_handle_window_event(&mut self, window: &Window, window_id: WindowId, event: WindowEvent) {
        self.imgui.handle_window_event(window, window_id, event);
    }

    pub fn render_block(&mut self, block: Block, glam::Vec3 { x, y, z }: glam::Vec3) {
        let vertices = [
            // -z
            Vertex { position: [x      , y + 1.0, z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y      , z      ], tex_coords: [0.0, 1.0] },
            Vertex { position: [x + 1.0, y      , z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y + 1.0, z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y      , z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z      ], tex_coords: [1.0, 0.0] },

            // -x
            Vertex { position: [x + 1.0, y + 1.0, z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y      , z      ], tex_coords: [0.0, 1.0] },
            Vertex { position: [x + 1.0, y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z - 1.0], tex_coords: [1.0, 0.0] },

            // +z
            Vertex { position: [x + 1.0, y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y      , z - 1.0], tex_coords: [0.0, 1.0] },
            Vertex { position: [x      , y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y + 1.0, z - 1.0], tex_coords: [1.0, 0.0] },

            // +x
            Vertex { position: [x      , y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y      , z - 1.0], tex_coords: [0.0, 1.0] },
            Vertex { position: [x      , y      , z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y      , z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y + 1.0, z      ], tex_coords: [1.0, 0.0] },

            // -y
            Vertex { position: [x      , y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y + 1.0, z      ], tex_coords: [0.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y + 1.0, z - 1.0], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y + 1.0, z      ], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y + 1.0, z - 1.0], tex_coords: [1.0, 0.0] },

            // +y
            Vertex { position: [x      , y      , z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x      , y      , z - 1.0], tex_coords: [0.0, 1.0] },
            Vertex { position: [x + 1.0, y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x      , y      , z      ], tex_coords: [0.0, 0.0] },
            Vertex { position: [x + 1.0, y      , z - 1.0], tex_coords: [1.0, 1.0] },
            Vertex { position: [x + 1.0, y      , z      ], tex_coords: [1.0, 0.0] },
        ];

        self.blocks.push(
            self.device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some(&format!("Block at {:.2}", glam::vec3(x, y, z))),
                contents: bytemuck::cast_slice(&vertices),
                usage: wgpu::BufferUsages::VERTEX,
            })
        );
    }

    fn draw_blocks(&self, render_pass: &mut wgpu::RenderPass) {
        for block in &self.blocks {
            render_pass.set_vertex_buffer(0, block.slice(..));
            render_pass.draw(0..36, 0..1);
        }
    }

    // there should not be a window here, imgui state is a messed up struct
    pub fn draw(&mut self, window: &Window) -> anyhow::Result<()> {
        let surface_texture = self.surface.get_current_texture()?;
        let texture_view = surface_texture.texture.create_view(&wgpu::TextureViewDescriptor::default());

        let mut encoder = self.device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("Render encoder"),
        });

        let mut render_pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
            label: Some("Render pass"),
            color_attachments: &[Some(wgpu::RenderPassColorAttachment {
                view: &texture_view,
                depth_slice: None,
                resolve_target: None,
                ops: wgpu::Operations {
                    load: wgpu::LoadOp::Clear(wgpu::Color {
                        r: 0.1,
                        g: 0.2,
                        b: 0.3,
                        a: 1.0,
                    }),
                    store: wgpu::StoreOp::Store,
                },
            })],
            depth_stencil_attachment: Some(wgpu::RenderPassDepthStencilAttachment {
                view: &self.depth_texture.view,
                depth_ops: Some(wgpu::Operations {
                    load: wgpu::LoadOp::Clear(1.0),
                    store: wgpu::StoreOp::Store,
                }),
                stencil_ops: None,
            }),
            timestamp_writes: None,
            occlusion_query_set: None,
            multiview_mask: None,
        });

        render_pass.set_pipeline(&self.render_pipeline);
        render_pass.set_bind_group(0, &self.diffuse_bind_group, &[]);
        render_pass.set_bind_group(1, &self.camera_bind_group, &[]);
        render_pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
        render_pass.set_index_buffer(self.index_buffer.slice(..), wgpu::IndexFormat::Uint16);
        render_pass.draw_indexed(0..(INDICES.len() as u32), 0, 0..1);

        self.draw_blocks(&mut render_pass);

        self.imgui.render(
            window,
            &self.device,
            &self.queue,
            &mut render_pass,
            self.imgui_fn.as_mut().unwrap(),
        );

        drop(render_pass);

        self.queue.submit(std::iter::once(encoder.finish()));
        surface_texture.present();

        Ok(())
    }
}

fn get_surface_config(surface_format: wgpu::TextureFormat, size: winit::dpi::PhysicalSize<u32>) -> wgpu::SurfaceConfiguration {
    wgpu::SurfaceConfiguration {
        usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
        format: surface_format,
        width: size.width,
        height: size.height,
        present_mode: wgpu::PresentMode::Fifo, // https://github.com/gfx-rs/wgpu/issues/8310
        desired_maximum_frame_latency: 2,
        alpha_mode: wgpu::CompositeAlphaMode::Auto,
        view_formats: vec![],
    }
}