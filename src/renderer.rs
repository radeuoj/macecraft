use glam::*;
use std::collections::HashMap;
use std::sync::Arc;
use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;
use winit::event::WindowEvent;
use winit::window::Window;
use crate::block::{Block, BlockFace};
use crate::chunk::Chunk;
use crate::camera::Camera;
use crate::imgui_renderer::ImGuiRenderer;
use crate::texture::{DepthTexture, Texture};
use crate::world::World;

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
    fn decompress(vertex: u32) -> Self {
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

    const fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 1] = wgpu::vertex_attr_array![
            0 => Uint32,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<u32>() as _,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct UiVertex {
    position: [f32; 2],
    tex_coords: [u32; 2],
}

impl UiVertex {
    const fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 2] = wgpu::vertex_attr_array![
            0 => Float32x2,
            1 => Uint32x2,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<UiVertex>() as _,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct ColorVertex {
    position: [f32; 3],
    color: [f32; 4],
}

impl ColorVertex {
    const fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 2] = wgpu::vertex_attr_array![
            0 => Float32x3,
            1 => Float32x4,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<ColorVertex>() as _,
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
            view_proj: Mat4::IDENTITY.to_cols_array_2d(),
        }
    }

    fn update_view_proj(&mut self, camera: &Camera) {
        self.view_proj = camera.build_view_proj_matrix().to_cols_array_2d();
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct ChunkPosUniform {
    chunk_pos: [i32; 3],
}

impl ChunkPosUniform {
    fn new() -> Self {
        Self { chunk_pos: [0; 3] }
    }

    fn set(&mut self, pos: IVec3) {
        self.chunk_pos = pos.into();
    }
}

pub struct Renderer {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: wgpu::Surface<'static>,
    surface_format: wgpu::TextureFormat,

    chunk_render_pipeline: wgpu::RenderPipeline,
    ui_render_pipeline: wgpu::RenderPipeline,
    color_render_pipeline: wgpu::RenderPipeline,

    diffuse_bind_group: wgpu::BindGroup,

    camera_uniform: CameraUniform,
    camera_buffer: wgpu::Buffer,
    camera_bind_group: wgpu::BindGroup,

    chunk_pos_buffer: wgpu::Buffer,
    chunk_bind_group: wgpu::BindGroup,
    chunk_bind_group_layout: wgpu::BindGroupLayout,

    depth_texture: DepthTexture,

    chunks: HashMap<IVec3, wgpu::Buffer>,

    has_target_block: bool,
    color_buffer: wgpu::Buffer,
    ui_buffer: wgpu::Buffer,

    imgui: ImGuiRenderer,
    imgui_content: Box<dyn FnOnce(&dear_imgui_rs::Ui)>,
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
                label: Some("Camera bind group layout"),
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
        
        let chunk_pos_buffer = create_chunk_pos_buffer(&device, World::MAX_CHUNKS);

        let chunk_bind_group_layout = device
            .create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: Some("Chunk bind group layout"),
                entries: &[
                    wgpu::BindGroupLayoutEntry {
                        binding: 0,
                        visibility: wgpu::ShaderStages::VERTEX,
                        ty: wgpu::BindingType::Buffer {
                            ty: wgpu::BufferBindingType::Uniform,
                            has_dynamic_offset: true,
                            min_binding_size: None,
                        },
                        count: None,
                    },
                ],
            });

        let chunk_bind_group = create_chunk_pos_bind_ground(
            &device, 
            &chunk_bind_group_layout,  
            &chunk_pos_buffer,
        );

        let color_buffer = device.create_buffer(
            &wgpu::BufferDescriptor {
                label: Some("Color vertex buffer"),
                size: (size_of::<ColorVertex>() * 8 * 3) as _,
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                mapped_at_creation: false,
            }
        );

        let ui_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Chunk vertex buffer"),
                contents: bytemuck::cast_slice(&render_crosshair(size.into())),
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
            }
        );

        let depth_texture = DepthTexture::new(&device, size.into(), "Depth texture");

        let chunk_shader = device
            .create_shader_module(wgpu::include_wgsl!("shaders/chunk.wgsl"));

        let chunk_render_pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Render pipeline layout"),
                bind_group_layouts: &[
                    &texture_bind_group_layout,
                    &camera_bind_group_layout,
                    &chunk_bind_group_layout,
                ],
                immediate_size: 0,
            });

        let chunk_render_pipeline = make_render_pipeline(
            "Render pipeline",
            &device, 
            &chunk_render_pipeline_layout, 
            &chunk_shader, 
            surface_format,
            Vertex::layout(),
            wgpu::PrimitiveTopology::TriangleList,
        );

        let ui_shader = device
            .create_shader_module(wgpu::include_wgsl!("shaders/ui.wgsl"));

        let ui_render_pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("UI render pipeline layout"),
                bind_group_layouts: &[&texture_bind_group_layout],
                immediate_size: 0,
            });

        let ui_render_pipeline = make_render_pipeline(
            "UI render pipeline",
            &device,
            &ui_render_pipeline_layout,
            &ui_shader,
            surface_format,
            UiVertex::layout(),
            wgpu::PrimitiveTopology::TriangleList,
        );

        let color_shader = device
            .create_shader_module(wgpu::include_wgsl!("shaders/color.wgsl"));

        let color_render_pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Color render pipeline layout"),
                bind_group_layouts: &[
                    &texture_bind_group_layout,
                    &camera_bind_group_layout,
                ],
                immediate_size: 0,
            });

        let color_render_pipeline = make_render_pipeline(
            "Color render pipeline",
            &device,
            &color_render_pipeline_layout,
            &color_shader,
            surface_format,
            ColorVertex::layout(),
            wgpu::PrimitiveTopology::LineList,
        );

        let imgui = ImGuiRenderer::new(&device, &queue, surface_format, &window);

        Self {
            device,
            queue,
            surface,
            surface_format,
            chunk_render_pipeline,
            ui_render_pipeline,
            color_render_pipeline,
            diffuse_bind_group,
            camera_uniform,
            camera_buffer,
            camera_bind_group,
            chunk_pos_buffer,
            chunk_bind_group,
            chunk_bind_group_layout,
            depth_texture,
            chunks: HashMap::new(),
            has_target_block: false,
            color_buffer,
            ui_buffer,
            imgui,
            imgui_content: Box::new(|_| {}),
        }
    }

    pub fn resize(&mut self, size: winit::dpi::PhysicalSize<u32>) {
        self.surface.configure(&self.device, &get_surface_config(self.surface_format, size));
        self.depth_texture = DepthTexture::new(&self.device, size.into(), "Depth texture");
        self.queue.write_buffer(&self.ui_buffer, 0, 
            bytemuck::cast_slice(&render_crosshair(size.into())));
    }

    pub fn update_camera(&mut self, camera: &Camera) {
        self.camera_uniform.view_proj = camera.build_view_proj_matrix().to_cols_array_2d();
        self.queue.write_buffer(&self.camera_buffer, 0, 
            bytemuck::cast_slice(&[self.camera_uniform]));
    }

    pub fn update_target_block(&mut self, pos: Option<IVec3>) {
        match pos {
            Some(pos) => {
                self.queue.write_buffer(&self.color_buffer, 0, 
                bytemuck::cast_slice(&render_target_block(pos)));
                self.has_target_block = true;
            }
            None => self.has_target_block = false,
        }
    }

    pub fn update_imgui(&mut self, content: impl FnOnce(&dear_imgui_rs::Ui) + 'static) {
        self.imgui_content = Box::new(content);
    }

    pub fn render_chunk(&mut self, pos: IVec3, world: &World) {
        let Some(chunk) = world.get_chunk(pos) else {
            self.chunks.remove(&pos);
            return;
        };

        let mut vertices: Vec<Vertex> = vec![];

        for x in 0..Chunk::SIZE as u8 {
            for y in 0..Chunk::SIZE as u8 {
                for z in 0..Chunk::SIZE as u8 {
                    let local_pos = u8vec3(x, y, z).as_ivec3();
                    let block = chunk.get(local_pos).unwrap();
                    if block == Block::AIR { continue }

                    let is_air = |local_pos| {
                        is_air_in_chunk_or_fallback(pos, local_pos, chunk, world)  
                    };

                    // -z
                    let (tx, ty) = block.get_texture_coords(BlockFace::ZN);
                    if is_air(local_pos + ivec3(0, 0, 1)) {
                        vertices.extend([
                            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x    , y    , z + 1], tex_coords: [tx    , ty + 1] },
                            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty    ] },
                        ]);
                    }

                    // -x
                    let (tx, ty) = block.get_texture_coords(BlockFace::XN);
                    if is_air(local_pos + ivec3(1, 0, 0)) {
                        vertices.extend([
                            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x + 1, y    , z + 1], tex_coords: [tx    , ty + 1] },
                            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },
                        ]);
                    }

                    // +z
                    let (tx, ty) = block.get_texture_coords(BlockFace::ZP);
                    if is_air(local_pos + ivec3(0, 0, -1)) {
                        vertices.extend([
                            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x + 1, y    , z    ], tex_coords: [tx    , ty + 1] },
                            Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x    , y    , z    ], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx + 1, ty    ] },
                        ]);
                    }

                    // +x
                    let (tx, ty) = block.get_texture_coords(BlockFace::XP);
                    if is_air(local_pos + ivec3(-1, 0, 0)) {
                        vertices.extend([
                            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x    , y    , z    ], tex_coords: [tx    , ty + 1] },
                            Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x    , y    , z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx + 1, ty    ] },
                        ]);
                    }

                    // -y
                    let (tx, ty) = block.get_texture_coords(BlockFace::YN);
                    if is_air(local_pos + ivec3(0, 1, 0)) {
                        vertices.extend([
                            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x    , y + 1, z + 1], tex_coords: [tx    , ty + 1] },
                            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x    , y + 1, z    ], tex_coords: [tx    , ty    ] },
                            Vertex { position: [x + 1, y + 1, z + 1], tex_coords: [tx + 1, ty + 1] },
                            Vertex { position: [x + 1, y + 1, z    ], tex_coords: [tx + 1, ty    ] },
                        ]);
                    }

                    // +y
                    let (tx, ty) = block.get_texture_coords(BlockFace::YP);
                    if is_air(local_pos + ivec3(0, -1, 0)) {
                        vertices.extend([
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
        }

        let vertices = vertices.iter().map(|v| v.compress()).collect::<Vec<_>>();

        self.chunks.insert(pos, self.device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Chunk vertex buffer"),
                contents: bytemuck::cast_slice(&vertices),
                usage: wgpu::BufferUsages::VERTEX,
            }
        ));

        self.try_resize_chunk_pos_buffer();
    }

    fn try_resize_chunk_pos_buffer(&mut self) {
        let buf_count = chunk_pos_buffer_count(&self.device, &self.chunk_pos_buffer);

        if self.chunks.len() > buf_count {
            self.chunk_pos_buffer = create_chunk_pos_buffer(&self.device, buf_count * 2);
            self.chunk_bind_group = create_chunk_pos_bind_ground(
                &self.device,
                &self.chunk_bind_group_layout,
                &self.chunk_pos_buffer,
            );
        }
    }

    fn draw_chunks(&mut self, render_pass: &mut wgpu::RenderPass) {
        for (i, (pos, chunk)) in self.chunks.iter()
                .filter(|(_, chunk)| chunk.size() > 0).enumerate() {
                    
            let mut chunk_pos_uniform = ChunkPosUniform::new();
            chunk_pos_uniform.set(*pos);
            let offset = i as u64 * self.device.limits().min_uniform_buffer_offset_alignment as u64;
            self.queue.write_buffer(&self.chunk_pos_buffer, offset, bytemuck::cast_slice(&[chunk_pos_uniform]));
            render_pass.set_bind_group(2, &self.chunk_bind_group, &[offset as u32]);
            render_pass.set_vertex_buffer(0, chunk.slice(..));
            render_pass.draw(0..(chunk.size() as u32 / 4), 0..1);
        }
    }

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

        render_pass.set_pipeline(&self.chunk_render_pipeline);
        render_pass.set_bind_group(0, &self.diffuse_bind_group, &[]);
        render_pass.set_bind_group(1, &self.camera_bind_group, &[]);

        self.draw_chunks(&mut render_pass);

        if self.has_target_block {
            render_pass.set_pipeline(&self.color_render_pipeline);
            render_pass.set_vertex_buffer(0, self.color_buffer.slice(..));
            render_pass.draw(0..(3 * 8), 0..1);
        }

        render_pass.set_pipeline(&self.ui_render_pipeline);
        render_pass.set_vertex_buffer(0, self.ui_buffer.slice(..));
        render_pass.draw(0..6, 0..1);

        let imgui_content = std::mem::replace(&mut self.imgui_content, Box::new(|_| {}));
        self.imgui.render(window, &mut render_pass, imgui_content);

        drop(render_pass);

        self.queue.submit(std::iter::once(encoder.finish()));
        surface_texture.present();

        Ok(())
    }

    pub fn handle_imgui_window_event(&mut self, window: &Window, event: &WindowEvent) {
        self.imgui.handle_window_event(window, event);
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

fn make_render_pipeline(
    label: &str,
    device: &wgpu::Device, 
    layout: &wgpu::PipelineLayout, 
    shader: &wgpu::ShaderModule, 
    surface_format: wgpu::TextureFormat,
    vertex_buffer_layout: wgpu::VertexBufferLayout,
    topology: wgpu::PrimitiveTopology,
) -> wgpu::RenderPipeline {
    device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
        label: Some(label),
        layout: Some(layout),
        vertex: wgpu::VertexState {
            module: &shader,
            entry_point: Some("vs_main"),
            compilation_options: Default::default(),
            buffers: &[vertex_buffer_layout],
        },
        fragment: Some(wgpu::FragmentState {
            module: &shader,
            entry_point: Some("fs_main"),
            compilation_options: Default::default(),
            targets: &[Some(wgpu::ColorTargetState {
                format: surface_format,
                blend: Some(wgpu::BlendState::PREMULTIPLIED_ALPHA_BLENDING),
                write_mask: wgpu::ColorWrites::ALL,
            })],
        }),
        primitive: wgpu::PrimitiveState {
            topology,
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
    })
}

fn render_crosshair(window_size: (u32, u32)) -> Vec<UiVertex> {
    const CROSSHAIR_SIZE: f32 = 40.0;

    let (tx, ty) = (5, 0);
    let dx = CROSSHAIR_SIZE / window_size.0 as f32 * 2.0;
    let dy = CROSSHAIR_SIZE / window_size.1 as f32 * 2.0;
    let (x, y) = (-dx / 2.0, -dy / 2.0);

    vec![
        UiVertex { position: [x     , y + dy], tex_coords: [tx    , ty    ] },
        UiVertex { position: [x     , y     ], tex_coords: [tx    , ty + 1] },
        UiVertex { position: [x + dx, y     ], tex_coords: [tx + 1, ty + 1] },
        UiVertex { position: [x     , y + dy], tex_coords: [tx    , ty    ] },
        UiVertex { position: [x + dx, y     ], tex_coords: [tx + 1, ty + 1] },
        UiVertex { position: [x + dx, y + dy], tex_coords: [tx + 1, ty    ] },
    ]
}

fn render_target_block(pos: IVec3) -> Vec<ColorVertex> {
    let pos = pos.as_vec3() - 0.001;
    let color = [0.0, 0.0, 0.0, 1.0];
    
    vec![
        ColorVertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },

        ColorVertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },
        ColorVertex { position: [pos.x        , pos.y        , pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },

        ColorVertex { position: [pos.x        , pos.y        , pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        ColorVertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },
        ColorVertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },
        ColorVertex { position: [pos.x        , pos.y        , pos.z        ], color },
    ]
}

fn create_chunk_pos_buffer(device: &wgpu::Device, chunks: usize) -> wgpu::Buffer {
    assert!(size_of::<ChunkPosUniform>() as u32 <= device.limits().min_uniform_buffer_offset_alignment);
    device.create_buffer(&wgpu::BufferDescriptor {
        label: Some("Chunk pos buffer"),
        size: device.limits().min_uniform_buffer_offset_alignment as u64 * chunks as u64,
        usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        mapped_at_creation: false,
    })
}

fn chunk_pos_buffer_count(device: &wgpu::Device, buf: &wgpu::Buffer) -> usize {
    buf.size() as usize / device.limits().min_uniform_buffer_offset_alignment as usize
}

fn create_chunk_pos_bind_ground(device: &wgpu::Device, layout: &wgpu::BindGroupLayout, buf: &wgpu::Buffer) -> wgpu::BindGroup {
    device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: Some("Chunk bind group"),
        layout: layout,
        entries: &[
            wgpu::BindGroupEntry {
                binding: 0,
                resource: buf.slice(0..device.limits().min_uniform_buffer_offset_alignment as u64).into(),
            },
        ],
    })
}

fn get_block_in_chunk_or_fallback(mut chunk_pos: IVec3, mut local_pos: IVec3, chunk: &Chunk, world: &World) -> Block {
    match chunk.get(local_pos) {
        Some(block) => block,
        None => {
            for i in 0..3 {
                if local_pos[i] < 0 {
                    local_pos[i] += Chunk::SIZE as i32;
                    chunk_pos[i] -= 1;
                }
            }

            let global_pos = World::chunk_pos_to_world_pos(chunk_pos, local_pos.as_uvec3());
            world.get_block(global_pos)
        }
    }
}

fn is_air_in_chunk_or_fallback(chunk_pos: IVec3, local_pos: IVec3, chunk: &Chunk, world: &World) -> bool {
    get_block_in_chunk_or_fallback(chunk_pos, local_pos, chunk, world) == Block::AIR
}