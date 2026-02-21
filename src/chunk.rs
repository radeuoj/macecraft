#[derive(Debug, Copy, Clone, PartialEq)]
pub struct Block(pub u8);

#[derive(Debug, Copy, Clone, PartialEq)]
pub enum BlockFace {
    ZN, // Z-
    ZP, // Z+
    XN, // X-
    XP, // X+
    YN, // Y-
    YP, // Y+
}

impl Block {
    pub const AIR: Block = Block(0);
    pub const DIRT: Block = Block(1);
    pub const GRASS: Block = Block(2);

    pub fn get_texture_coords(&self, face: BlockFace) -> (u8, u8) {
        match self {
            &Self::AIR => (0, 0),
            &Self::DIRT => (1, 0),
            &Self::GRASS => match face {
                BlockFace::YN => (3, 0),
                BlockFace::YP => (1, 0),
                _ => (2, 0),
            }
            _ => unimplemented!("invalid block"),
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
            blocks: Box::new([Block::AIR; _]),
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
                    self.set(glam::uvec3(x, y, z), Block::DIRT);
                }
            }
        }

        for x in 0..Self::SIZE {
            for z in 0..Self::SIZE {
                self.set(glam::uvec3(x, 4, z), Block::GRASS);
            }
        }
    }
}