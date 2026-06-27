mod chunk_mesh;
mod chunk;
mod outline;
mod ui;
mod imgui;

use glam::*;
use std::sync::Arc;
use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;
use winit::event::WindowEvent;
use winit::window::Window;

use crate::camera::Camera;
use crate::renderer::chunk::ChunkRenderer;
use crate::renderer::imgui::ImGuiRenderer;
use crate::renderer::outline::OutlineRenderer;
use crate::renderer::ui::UiRenderer;
use crate::texture::{DepthTexture, Texture};
use crate::world::World;

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

pub struct Renderer {
    device: wgpu::Device,
    queue: wgpu::Queue,
    surface: wgpu::Surface<'static>,
    surface_format: wgpu::TextureFormat,

    texture_bind_group: wgpu::BindGroup,
    camera_uniform: CameraUniform,
    camera_buffer: wgpu::Buffer,
    camera_bind_group: wgpu::BindGroup,
    depth_texture: DepthTexture,

    chunks: ChunkRenderer,
    outline: OutlineRenderer,
    ui: UiRenderer,
    imgui: ImGuiRenderer,
}

impl Renderer {
    pub async fn new(window: Arc<Window>, camera: &Camera) -> Self {
        let instance = wgpu::Instance::new(&wgpu::InstanceDescriptor::from_env_or_default());

        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                ..Default::default()
            })
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

        let texture_bytes = include_bytes!("../textures/textures.png");
        let texture = Texture::from_bytes(
            &device,
            &queue,
            texture_bytes,
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

        let texture_bind_group = device
            .create_bind_group(&wgpu::BindGroupDescriptor {
                label: Some("Texture bind group"),
                layout: &texture_bind_group_layout,
                entries: &[
                    wgpu::BindGroupEntry {
                        binding: 0,
                        resource: wgpu::BindingResource::TextureView(&texture.view),
                    },
                    wgpu::BindGroupEntry {
                        binding: 1,
                        resource: wgpu::BindingResource::Sampler(&texture.sampler),
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

        let depth_texture = DepthTexture::new(&device, size.into(), "Depth texture");

        let chunks = ChunkRenderer::new(
            &device, 
            &texture_bind_group_layout, 
            &camera_bind_group_layout, 
            surface_format,
        );

        let outline = OutlineRenderer::new(
            &device, 
            &camera_bind_group_layout, 
            surface_format,
        );

        let ui = UiRenderer::new(
            &device, 
            size.into(), 
            &texture_bind_group_layout, 
            surface_format,
        );

        let imgui = ImGuiRenderer::new(&device, &queue, surface_format, &window);

        Self {
            device,
            queue,
            surface,
            surface_format,
            chunks,
            outline,
            ui,
            texture_bind_group,
            camera_uniform,
            camera_buffer,
            camera_bind_group,
            depth_texture,
            imgui,
        }
    }

    pub fn resize(&mut self, size: winit::dpi::PhysicalSize<u32>) {
        self.surface.configure(&self.device, &get_surface_config(self.surface_format, size));
        self.depth_texture = DepthTexture::new(&self.device, size.into(), "Depth texture");
        self.ui.resize(&self.queue, size.into());
    }

    pub fn update_camera(&mut self, camera: &Camera) {
        self.camera_uniform.view_proj = camera.build_view_proj_matrix().to_cols_array_2d();
        self.queue.write_buffer(&self.camera_buffer, 0, 
            bytemuck::cast_slice(&[self.camera_uniform]));
    }

    pub fn upload_chunk(&mut self, pos: IVec3, world: &World) {
        self.chunks.upload(pos, world, &self.device);
    }

    pub fn set_target_block(&mut self, pos: Option<IVec3>) {
        self.outline.set_target_block(pos, &self.queue);
    }

    pub fn update_imgui(&mut self, content: impl FnOnce(&dear_imgui_rs::Ui) + 'static) {
        self.imgui.set_content(content);
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
                        r: 0.471,
                        g: 0.655,
                        b: 1.000,
                        a: 1.000,
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

        self.chunks.draw_opaque(
            &mut render_pass, 
            &self.device, 
            &self.queue, 
            &self.texture_bind_group, 
            &self.camera_bind_group,
        );

        self.outline.draw(
            &mut render_pass,
            &self.camera_bind_group,
        );

        self.ui.draw(
            &mut render_pass, 
            &self.texture_bind_group,
        );

        self.imgui.draw(window, &mut render_pass);

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

fn create_render_pipeline(
    label: &str,
    device: &wgpu::Device, 
    layout: &wgpu::PipelineLayout, 
    shader: &wgpu::ShaderModule, 
    surface_format: wgpu::TextureFormat,
    vertex_buffer_layout: wgpu::VertexBufferLayout,
    topology: wgpu::PrimitiveTopology,
    depth_write_enabled: bool,
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
            depth_write_enabled,
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