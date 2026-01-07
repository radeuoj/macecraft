#[derive(Debug, Copy, Clone, PartialEq)]
pub enum Block {
    Air,
    Dirt,
    Grass,
}

impl From<u8> for Block {
    fn from(value: u8) -> Self {
        use Block::*;

        match value {
            0 => Air,
            1 => Dirt,
            2 => Grass,
            _ => unreachable!(),
        }
    }
}

pub struct Chunk {
    blocks: Box<[Block; Self::SIZE.pow(3) as usize]>,
}

impl Chunk {
    pub const SIZE: u32 = 32;

    pub fn new() -> Self {
        Self {
            blocks: Box::new([Block::Air; _]),
        }
    }

    /// assumes pos is in bounds
    pub fn get(&self, pos: glam::UVec3) -> Block {
        self.blocks[(pos.x + pos.y * Self::SIZE + pos.z * Self::SIZE.pow(2)) as usize]
    }

    /// assumes pos is in bounds
    pub fn set(&mut self, pos: glam::UVec3, block: Block) {
        self.blocks[(pos.x + pos.y * Self::SIZE + pos.z * Self::SIZE.pow(2)) as usize] = block;
    }

    pub fn generate_superflat(&mut self) {
        for y in 0..4u32 {
            for x in 0..Self::SIZE {
                for z in 0..Self::SIZE {
                    self.set(glam::uvec3(x, y, z), Block::Dirt);
                }
            }
        }

        for x in 0..Self::SIZE {
            for z in 0..Self::SIZE {
                self.set(glam::uvec3(x, 4, z), Block::Grass);
            }
        }
    }
}