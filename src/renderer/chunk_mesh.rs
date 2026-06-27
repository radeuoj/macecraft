use glam::*;
use wgpu::util::DeviceExt;

use crate::{block::{Block, BlockFace}, chunk::Chunk, renderer::chunk::Vertex, world::World};

pub struct ChunkMesh {
    pub opaque: wgpu::Buffer,
    pub translucent: wgpu::Buffer,
}

impl ChunkMesh {
    /**
     * returns none if the chunk doesn't exist
     */
    pub fn build(pos: IVec3, world: &World, device: &wgpu::Device) -> Option<Self> {
        let Some(chunk) = world.get_chunk(pos) else {
            return None;
        };

        let mut opaque_vertices: Vec<Vertex> = vec![];
        let mut translucent_vertices: Vec<Vertex> = vec![];

        for x in 0..Chunk::SIZE as u8 {
            for y in 0..Chunk::SIZE as u8 {
                for z in 0..Chunk::SIZE as u8 {
                    let local_pos = u8vec3(x, y, z).as_ivec3();
                    let block = chunk.get(local_pos).unwrap();
                    if block == Block::AIR { continue }

                    let is_air = |local_pos| {
                        let target = get_block_in_chunk_or_fallback(pos, local_pos, chunk, world);
                        target == Block::AIR ||
                        (block.is_opaque() && target.is_translucent())
                    };

                    let vertices = if block.is_opaque() { &mut opaque_vertices } 
                        else { &mut translucent_vertices };

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

        let opaque_vertices = opaque_vertices.iter().map(|v| v.compress()).collect::<Vec<_>>();
        let translucent_vertices = translucent_vertices.iter().map(|v| v.compress()).collect::<Vec<_>>();

        let opaque_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Chunk opaque vertex buffer"),
                contents: bytemuck::cast_slice(&opaque_vertices),
                usage: wgpu::BufferUsages::VERTEX,
            }
        );

        let translucent_buffer = device.create_buffer_init(
            &wgpu::util::BufferInitDescriptor {
                label: Some("Chunk translucent vertex buffer"),
                contents: bytemuck::cast_slice(&translucent_vertices),
                usage: wgpu::BufferUsages::VERTEX,
            }
        );

        Some(Self {
            opaque: opaque_buffer,
            translucent: translucent_buffer,
        })
    }
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