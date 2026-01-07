use std::sync::Arc;
use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;
use winit::event::WindowEvent;
use winit::window::{Window, WindowId};
use crate::chunk::{Block, Chunk};
use crate::camera::Camera;
use crate::imgui::ImGuiState;
use crate::texture::{DepthTexture, Texture};

#[derive(Debug)]
struct Vertex {
    position: [u8; 3], // chunk pos 6 bits each
    tex_coords: [u8; 2], // texture coords 5 bits each
}

impl Vertex {
    fn compress(&self) -> u32 {
        self.position[0] as u32
            | ((self.position[1] as u32) << 6)
            | ((self.position[2] as u32) << 12)
            | ((self.tex_coords[0] as u32) << 18)
            | ((self.tex_coords[1] as u32) << 23)
    }

    #[allow(unused)]
    fn uncompress(vertex: u32) -> Self {
        Self {
            position: [
                (vertex & ((1 << 6) - 1)) as u8,
                ((vertex >> 6) & ((1 << 6) - 1)) as u8,
                ((vertex >> 12) & ((1 << 6) - 1)) as u8
            ],
            tex_coords: [
                ((vertex >> 18) & ((1 << 5) - 1)) as u8,
                ((vertex >> 23) & ((1 << 5) - 1)) as u8
            ],
        }
    }

    fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 1] = wgpu::vertex_attr_array![
            0 => Uint32,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<u32>() as wgpu::BufferAddress,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

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

    diffuse_bind_group: wgpu::BindGroup,

    camera_uniform: CameraUniform,
    camera_buffer: wgpu::Buffer,
    camera_bind_group: wgpu::BindGroup,

    depth_texture: DepthTexture,

    backend: wgpu::Backend,

    imgui: ImGuiState,
    imgui_fn: Option<Box<dyn FnMut(&mut imgui::Ui)>>,

    blocks: Vec<wgpu::Buffer>,
    chunk: Option<wgpu::Buffer>,
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

        let diffuse_bytes = include_bytes!("textures/textures.png");
        let diffuse_texture = Texture::from_bytes(
            &device,
            &queue,
            diffuse_bytes,
            "textures",
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
            diffuse_bind_group,
            camera_uniform,
            camera_buffer,
            camera_bind_group,
            depth_texture,
            backend: adapter.get_info().backend,
            imgui,
            imgui_fn: None,
            blocks: vec![],
            chunk: None,
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

    pub fn render_block(&mut self, block: Block, glam::U8Vec3 { x, y, z }: glam::U8Vec3) {
        let tx = block as u8 % 16;
        let ty = block as u8 / 16;

        let vertices = [
            // -z
            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty    ] },

            // -x
            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },

            // +z
            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx + 1, ty    ] },

            // +x
            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y    , z    ], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx + 1, ty    ] },

            // -y
            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },

            // +y
            Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x    , y    , z    ], tex_coords: [tx    , ty + 1] },
            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty    ] },
            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty    ] },
        ];

        let vertices = vertices.map(|v| v.compress());

        self.blocks.push(
            self.device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
                label: Some(&format!("Block at {:.2}", glam::u8vec3(x, y, z))),
                contents: bytemuck::cast_slice(&vertices),
                usage: wgpu::BufferUsages::VERTEX,
            })
        );
    }

    pub fn render_chunk(&mut self, chunk: &Chunk) {
        let mut vertices: Vec<Vertex> = vec![];

        for x in 0..Chunk::SIZE as u8 {
            for y in 0..Chunk::SIZE as u8 {
                for z in 0..Chunk::SIZE as u8 {
                    let block = chunk.get(glam::uvec3(x as u32, y as u32, z as u32));
                    if block == Block::Air { continue; }
                    let tx = block as u8 % 16;
                    let ty = block as u8 / 16;

                    vertices.extend([
                        // -z
                        Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty    ] },

                        // -x
                        Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },

                        // +z
                        Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y    , z    ], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y + 1, z    ], tex_coords: [tx + 1, ty    ] },

                        // +x
                        Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y    , z    ], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx + 1, ty    ] },

                        // -y
                        Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },

                        // +y
                        Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x    , y    , z    ], tex_coords: [tx    , ty + 1] },
                        Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty    ] },
                        Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                        Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty    ] },
                    ]);
                }
            }
        }

        let vertices = vertices.iter().map(|v| v.compress()).collect::<Vec<_>>();

        self.chunk = Some(self.device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Chunk vertex buffer"),
                contents: bytemuck::cast_slice(&vertices),
                usage: wgpu::BufferUsages::VERTEX,
            }
        ));
    }

    fn draw_blocks(&self, render_pass: &mut wgpu::RenderPass) {
        for block in &self.blocks {
            render_pass.set_vertex_buffer(0, block.slice(..));
            render_pass.draw(0..36, 0..1);
        }
    }

    fn draw_chunk(&self, render_pass: &mut wgpu::RenderPass) {
        if let Some(ref chunk) = self.chunk {
            render_pass.set_vertex_buffer(0, chunk.slice(..));
            render_pass.draw(0..(chunk.size() as u32 / 4), 0..1);
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

        self.draw_chunk(&mut render_pass);
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