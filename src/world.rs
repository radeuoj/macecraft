use std::collections::HashMap;

use crate::chunk::{Block, Chunk};

pub struct World {
    chunks: HashMap<glam::IVec3, Chunk>,
}

impl World {
    pub const MAX_CHUNKS: usize = 256;

    pub fn new() -> Self {
        Self {
            chunks: HashMap::new(),
        }
    }

    pub fn add_chunk(&mut self, pos: glam::IVec3, chunk: Chunk) {
        self.chunks.insert(pos, chunk);
    }

    pub fn get_chunk(&self, pos: glam::IVec3) -> Option<&Chunk> {
        self.chunks.get(&pos)
    }

    pub fn get_block(&self, pos: glam::IVec3) -> Block {
        let (chunk_pos, local_pos) = World::world_pos_to_chunk_pos(pos);

        match self.chunks.get(&chunk_pos) {
            Some(chunk) => chunk.get(local_pos),
            None => Block::Air,
        }
    }

    pub fn is_air(&self, pos: glam::IVec3) -> bool {
        self.get_block(pos) == Block::Air
    }

    /**
     * returns (chunk_pos, local_pos)
     */
    pub fn world_pos_to_chunk_pos(pos: glam::IVec3) -> (glam::IVec3, glam::UVec3) {
        let mut chunk_pos = pos / Chunk::SIZE as i32;
        let mut local_pos = pos % Chunk::SIZE as i32;
        let is_neg = local_pos.cmplt(glam::IVec3::ZERO);
        chunk_pos -= glam::IVec3::select(is_neg, glam::IVec3::ONE, glam::IVec3::ZERO);
        local_pos += glam::IVec3::select(is_neg, glam::IVec3::ONE * Chunk::SIZE as i32, glam::IVec3::ZERO);
        (chunk_pos, local_pos.as_uvec3())
    }

    pub fn chunk_pos_to_world_pos(chunk_pos: glam::IVec3, local_pos: glam::UVec3) -> glam::IVec3 {
        chunk_pos * Chunk::SIZE as i32 + local_pos.as_ivec3()
    }
}