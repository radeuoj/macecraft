use std::collections::HashMap;

use crate::chunk::Chunk;

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

    pub fn add_chunk(&mut self, pos: glam::IVec3, chunk: Chunk) -> &Chunk {
        self.chunks.insert(pos, chunk);
        self.chunks.get(&pos).unwrap()
    }
}