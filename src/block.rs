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
    pub const COBBLE: Block = Block(3);

    pub fn get_texture_coords(&self, face: BlockFace) -> (u8, u8) {
        match *self {
            Self::DIRT => (1, 0),
            Self::GRASS => match face {
                BlockFace::YN => (3, 0),
                BlockFace::YP => (1, 0),
                _ => (2, 0),
            }
            Self::COBBLE => (4, 0),
            _ => unimplemented!("invalid block {:?}", self),
        }
    }
}