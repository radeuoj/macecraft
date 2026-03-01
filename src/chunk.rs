use glam::*;
use crate::block::Block;

pub struct Chunk {
    blocks: Box<[Block; Self::SIZE.pow(3) as usize]>,
}

impl Chunk {
    pub const SIZE: u32 = 32;

    pub fn new() -> Self {
        Self {
            blocks: Box::new([Block::AIR; _]),
        }
    }

    /// assumes pos is in bounds
    pub unsafe fn get_unchecked(&self, pos: UVec3) -> Block {
        self.blocks[(pos.x + pos.y * Self::SIZE + pos.z * Self::SIZE.pow(2)) as usize]
    }

    /// assumes pos is in bounds
    pub unsafe fn set_unchecked(&mut self, pos: UVec3, block: Block) {
        self.blocks[(pos.x + pos.y * Self::SIZE + pos.z * Self::SIZE.pow(2)) as usize] = block;
    }

    // returns none if local_pos is out of bounds
    pub fn get(&self, local_pos: IVec3) -> Option<Block> {
        for i in 0..3 {
            if local_pos[i] < 0 || local_pos[i] >= Chunk::SIZE as i32 {
                return None;
            }
        }

        unsafe { Some(self.get_unchecked(local_pos.as_uvec3())) }
    }

    pub fn generate_superflat(&mut self) {
        for y in 0..4u32 {
            for x in 0..Self::SIZE {
                for z in 0..Self::SIZE {
                    unsafe {
                        self.set_unchecked(uvec3(x, y, z), Block::DIRT);
                    }
                }
            }
        }

        for x in 0..Self::SIZE {
            for z in 0..Self::SIZE {
                unsafe {
                    self.set_unchecked(uvec3(x, 4, z), Block::GRASS);
                }
            }
        }
    }

    pub fn generate_fill(&mut self, block: Block) {
        self.blocks.fill(block);
    }

    pub fn get_neighbours(pos: IVec3) -> [IVec3; 6] {
        [
            pos + ivec3( 0, -1,  0),
            pos + ivec3( 0,  1,  0),
            pos + ivec3(-1,  0,  0),
            pos + ivec3( 1,  0,  0),
            pos + ivec3( 0,  0, -1),
            pos + ivec3( 0,  0,  1),
        ]
    }
}