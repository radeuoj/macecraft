use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;

use crate::renderer::create_render_pipeline;

#[repr(C)]
#[derive(Copy, Clone, Debug, Pod, Zeroable)]
struct Vertex {
    position: [f32; 2],
    tex_coords: [u32; 2],
}

impl Vertex {
    const fn layout() -> wgpu::VertexBufferLayout<'static> {
        const ATTRIBUTES: [wgpu::VertexAttribute; 2] = wgpu::vertex_attr_array![
            0 => Float32x2,
            1 => Uint32x2,
        ];

        wgpu::VertexBufferLayout {
            array_stride: size_of::<Vertex>() as _,
            step_mode: wgpu::VertexStepMode::Vertex,
            attributes: &ATTRIBUTES,
        }
    }
}

pub struct UiRenderer {
    pipeline: wgpu::RenderPipeline,
    buffer: wgpu::Buffer,
}

impl UiRenderer {
    pub fn new(
        device: &wgpu::Device, 
        window_size: (u32, u32),
        texture_bind_group_layout: &wgpu::BindGroupLayout,
        surface_format: wgpu::TextureFormat,
    ) -> Self {
        let buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Ui vertex buffer"),
                contents: bytemuck::cast_slice(&render_crosshair(window_size)),
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
            }
        );

        let shader = device
            .create_shader_module(wgpu::include_wgsl!("../shaders/ui.wgsl"));

        let pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("UI render pipeline layout"),
                bind_group_layouts: &[texture_bind_group_layout],
                immediate_size: 0,
            });

        let pipeline = create_render_pipeline(
            "UI render pipeline",
            &device,
            &pipeline_layout,
            &shader,
            surface_format,
            Vertex::layout(),
            wgpu::PrimitiveTopology::TriangleList,
            true,
        );

        Self {
            pipeline,
            buffer,
        }
    }

    pub fn resize(&mut self, queue: &wgpu::Queue, window_size: (u32, u32)) {
        queue.write_buffer(&self.buffer, 0, 
            bytemuck::cast_slice(&render_crosshair(window_size)));
    }

    pub fn draw(
        &self,
        render_pass: &mut wgpu::RenderPass,
        texture_bind_group: &wgpu::BindGroup,
    ) {
        render_pass.set_pipeline(&self.pipeline);
        render_pass.set_bind_group(0, texture_bind_group, &[]);
        render_pass.set_vertex_buffer(0, self.buffer.slice(..));
        render_pass.draw(0..6, 0..1);
    }
}

fn render_crosshair(window_size: (u32, u32)) -> Vec<Vertex> {
    const CROSSHAIR_SIZE: f32 = 40.0;

    let (tx, ty) = (5, 0);
    let dx = CROSSHAIR_SIZE / window_size.0 as f32 * 2.0;
    let dy = CROSSHAIR_SIZE / window_size.1 as f32 * 2.0;
    let (x, y) = (-dx / 2.0, -dy / 2.0);

    vec![
        Vertex { position: [x     , y + dy], tex_coords: [tx    , ty    ] },
        Vertex { position: [x     , y     ], tex_coords: [tx    , ty + 1] },
        Vertex { position: [x + dx, y     ], tex_coords: [tx + 1, ty + 1] },
        Vertex { position: [x     , y + dy], tex_coords: [tx    , ty    ] },
        Vertex { position: [x + dx, y     ], tex_coords: [tx + 1, ty + 1] },
        Vertex { position: [x + dx, y + dy], tex_coords: [tx + 1, ty    ] },
    ]
}