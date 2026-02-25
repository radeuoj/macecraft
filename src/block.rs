use glam::*;

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

    pub fn get_neighbour(pos: IVec3, face: BlockFace) -> IVec3 {
        match face {
            BlockFace::ZN => pos + IVec3::Z,
            BlockFace::ZP => pos - IVec3::Z,
            BlockFace::XN => pos + IVec3::X,
            BlockFace::XP => pos - IVec3::X,
            BlockFace::YN => pos + IVec3::Y,
            BlockFace::YP => pos - IVec3::Y,
        }
    }
}