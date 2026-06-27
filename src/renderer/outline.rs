use glam::*;
use bytemuck::{Pod, Zeroable};

use crate::renderer::create_render_pipeline;

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct Vertex {
    position: [f32; 3],
    color: [f32; 4],
}

impl Vertex {
    const fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 2] = wgpu::vertex_attr_array![
            0 => Float32x3,
            1 => Float32x4,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<Vertex>() as _,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

pub struct OutlineRenderer {
    pipeline: wgpu::RenderPipeline,
    buffer: wgpu::Buffer,
    has_target_block: bool,
}

impl OutlineRenderer {
    pub fn new(
        device: &wgpu::Device, 
        camera_bind_group_layout: &wgpu::BindGroupLayout,
        surface_format: wgpu::TextureFormat,
    ) -> Self {
        let buffer = device.create_buffer(
            &wgpu::BufferDescriptor {
                label: Some("Outline vertex buffer"),
                size: (size_of::<Vertex>() * 8 * 3) as _,
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                mapped_at_creation: false,
            }
        );

        let shader = device
            .create_shader_module(wgpu::include_wgsl!("../shaders/color.wgsl"));

        let pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Color render pipeline layout"),
                bind_group_layouts: &[
                    &camera_bind_group_layout,
                ],
                immediate_size: 0,
            });

        let pipeline = create_render_pipeline(
            "Outline render pipeline",
            &device,
            &pipeline_layout,
            &shader,
            surface_format,
            Vertex::layout(),
            wgpu::PrimitiveTopology::LineList,
            true,
        );

        Self {
            pipeline,
            buffer,
            has_target_block: false,
        }
    }

    pub fn set_target_block(&mut self, pos: Option<IVec3>, queue: &wgpu::Queue) {
        match pos {
            Some(pos) => {
                queue.write_buffer(&self.buffer, 0, 
                bytemuck::cast_slice(&render_target_block(pos)));
                self.has_target_block = true;
            }
            None => self.has_target_block = false,
        }
    }

    pub fn draw(
        &self, 
        render_pass: &mut wgpu::RenderPass,
        camera_bind_group: &wgpu::BindGroup,
    ) {
        if self.has_target_block {
            render_pass.set_pipeline(&self.pipeline);
            render_pass.set_bind_group(0, camera_bind_group, &[]);
            render_pass.set_vertex_buffer(0, self.buffer.slice(..));
            render_pass.draw(0..(3 * 8), 0..1);
        }
    }
}

fn render_target_block(pos: IVec3) -> Vec<Vertex> {
    let pos = pos.as_vec3() - 0.001;
    let color = [0.0, 0.0, 0.0, 1.0];
    
    vec![
        Vertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },

        Vertex { position: [pos.x        , pos.y        , pos.z + 1.002], color },
        Vertex { position: [pos.x        , pos.y        , pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z + 1.002], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },

        Vertex { position: [pos.x        , pos.y        , pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y        , pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        Vertex { position: [pos.x + 1.002, pos.y + 1.002, pos.z        ], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },
        Vertex { position: [pos.x        , pos.y + 1.002, pos.z        ], color },
        Vertex { position: [pos.x        , pos.y        , pos.z        ], color },
    ]
}