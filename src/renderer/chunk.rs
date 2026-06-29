use std::collections::HashMap;

use bytemuck::{Pod, Zeroable};
use glam::IVec3;

use crate::{renderer::{chunk_mesh::ChunkMesh, create_render_pipeline}, world::World};

#[derive(Debug)]
pub struct Vertex {
    pub position: [u8; 3], // chunk pos 6 bits each
    pub tex_coords: [u8; 2], // texture coords 5 bits each
    pub top_block: bool, // top block 1 bit
}

impl Vertex {
    pub fn compress(&self) -> u32 {
        self.position[0] as u32
            | ((self.position[1] as u32) << 6)
            | ((self.position[2] as u32) << 12)
            | ((self.tex_coords[0] as u32) << 18)
            | ((self.tex_coords[1] as u32) << 23)
            | ((self.top_block as u32) << 28)
    }

    #[allow(unused)]
    pub fn decompress(vertex: u32) -> Self {
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
            top_block: (vertex >> 28) != 0,
        }
    }

    pub const fn layout() -> wgpu::VertexBufferLayout<'static> {
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

pub struct ChunkRenderer {
    opaque_pipeline: wgpu::RenderPipeline,
    translucent_pipeline: wgpu::RenderPipeline,

    chunk_pos_buffer: wgpu::Buffer,
    chunk_pos_bind_group: wgpu::BindGroup,
    chunk_pos_bind_group_layout: wgpu::BindGroupLayout,

    chunks: HashMap<IVec3, ChunkMesh>,
}

impl ChunkRenderer {
    pub fn new(
        device: &wgpu::Device, 
        texture_bind_group_layout: &wgpu::BindGroupLayout, 
        camera_bind_group_layout: &wgpu::BindGroupLayout,
        surface_format: wgpu::TextureFormat,
    ) -> Self {
        let chunk_pos_buffer = create_chunk_pos_buffer(device, World::MAX_CHUNKS);
        let chunk_pos_bind_group_layout = create_chunk_pos_bind_group_layout(device);
        let chunk_pos_bind_group = create_chunk_pos_bind_ground(
            device, 
            &chunk_pos_bind_group_layout,  
            &chunk_pos_buffer,
        );

        let shader = device
            .create_shader_module(wgpu::include_wgsl!("../shaders/chunk.wgsl"));

        let pipeline_layout = device
            .create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
                label: Some("Render pipeline layout"),
                bind_group_layouts: &[
                    &texture_bind_group_layout,
                    &camera_bind_group_layout,
                    &chunk_pos_bind_group_layout,
                ],
                immediate_size: 0,
            });

        let opaque_pipeline = create_render_pipeline(
            "Chunk opaque render pipeline",
            device, 
            &pipeline_layout, 
            &shader, 
            surface_format,
            Vertex::layout(),
            wgpu::PrimitiveTopology::TriangleList,
            true,
            Some(wgpu::Face::Back),
        );

        let translucent_pipeline = create_render_pipeline(
            "Chunk translucent render pipeline",
            device, 
            &pipeline_layout, 
            &shader, 
            surface_format,
            Vertex::layout(),
            wgpu::PrimitiveTopology::TriangleList,
            false,
            None,
        );

        Self {
            opaque_pipeline,
            translucent_pipeline,
            chunk_pos_buffer,
            chunk_pos_bind_group_layout,
            chunk_pos_bind_group,
            chunks: HashMap::new(),
        }
    }

    pub fn upload(&mut self, pos: IVec3, world: &World, device: &wgpu::Device) {
        let Some(mesh) = ChunkMesh::build(pos, world, device) else {
            self.chunks.remove(&pos);
            return;
        };

        self.chunks.insert(pos, mesh);
        self.try_resize_chunk_pos_buffer(device);
    }

    fn try_resize_chunk_pos_buffer(&mut self, device: &wgpu::Device) {
        let buf_count = get_chunk_pos_buffer_count(device, &self.chunk_pos_buffer);

        if self.chunks.len() > buf_count {
            self.chunk_pos_buffer = create_chunk_pos_buffer(device, buf_count * 2);
            self.chunk_pos_bind_group = create_chunk_pos_bind_ground(
                device,
                &self.chunk_pos_bind_group_layout,
                &self.chunk_pos_buffer,
            );
        }
    }

    pub fn draw_opaque(
        &mut self, 
        render_pass: &mut wgpu::RenderPass,
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        texture_bind_group: &wgpu::BindGroup,
        camera_bind_group: &wgpu::BindGroup,
    ) {
        render_pass.set_pipeline(&self.opaque_pipeline);
        render_pass.set_bind_group(0, texture_bind_group, &[]);
        render_pass.set_bind_group(1, camera_bind_group, &[]);

        for (i, (pos, buf)) in self.chunks.iter()
                .map(|(pos, mesh)| (pos, &mesh.opaque)).enumerate() {
            if buf.size() == 0 { continue }
                    
            let mut chunk_pos_uniform = ChunkPosUniform::new();
            chunk_pos_uniform.set(*pos);
            let offset = i as u64 * device.limits().min_uniform_buffer_offset_alignment as u64;
            queue.write_buffer(&self.chunk_pos_buffer, offset, bytemuck::cast_slice(&[chunk_pos_uniform]));
            render_pass.set_bind_group(2, &self.chunk_pos_bind_group, &[offset as u32]);
            render_pass.set_vertex_buffer(0, buf.slice(..));
            render_pass.draw(0..(buf.size() as u32 / 4), 0..1);
        }
    }

    pub fn draw_translucent(
        &mut self, 
        render_pass: &mut wgpu::RenderPass,
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        texture_bind_group: &wgpu::BindGroup,
        camera_bind_group: &wgpu::BindGroup,
    ) {
        render_pass.set_pipeline(&self.translucent_pipeline);
        render_pass.set_bind_group(0, texture_bind_group, &[]);
        render_pass.set_bind_group(1, camera_bind_group, &[]);

        for (i, (pos, buf)) in self.chunks.iter()
                .map(|(pos, mesh)| (pos, &mesh.translucent)).enumerate() {
            if buf.size() == 0 { continue }
                    
            let mut chunk_pos_uniform = ChunkPosUniform::new();
            chunk_pos_uniform.set(*pos);
            let offset = i as u64 * device.limits().min_uniform_buffer_offset_alignment as u64;
            queue.write_buffer(&self.chunk_pos_buffer, offset, bytemuck::cast_slice(&[chunk_pos_uniform]));
            render_pass.set_bind_group(2, &self.chunk_pos_bind_group, &[offset as u32]);
            render_pass.set_vertex_buffer(0, buf.slice(..));
            render_pass.draw(0..(buf.size() as u32 / 4), 0..1);
        }
    }
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

fn create_chunk_pos_bind_group_layout(device: &wgpu::Device) -> wgpu::BindGroupLayout {
    device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
        label: Some("Chunk pos bind group layout"),
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
    })
}

fn create_chunk_pos_bind_ground(device: &wgpu::Device, layout: &wgpu::BindGroupLayout, buf: &wgpu::Buffer) -> wgpu::BindGroup {
    device.create_bind_group(&wgpu::BindGroupDescriptor {
        label: Some("Chunk pos bind group"),
        layout: layout,
        entries: &[
            wgpu::BindGroupEntry {
                binding: 0,
                resource: buf.slice(0..device.limits().min_uniform_buffer_offset_alignment as u64).into(),
            },
        ],
    })
}

fn get_chunk_pos_buffer_count(device: &wgpu::Device, buf: &wgpu::Buffer) -> usize {
    buf.size() as usize / device.limits().min_uniform_buffer_offset_alignment as usize
}