use glam::*;

use crate::block::Block;

pub trait TerrainGen {
    fn get(&self, position: IVec3) -> Block;
}

pub struct SuperflatGen;

impl TerrainGen for SuperflatGen {
    fn get(&self, position: IVec3) -> Block {
        const H: i32 = 4;

        match position.y {
            H => Block::GRASS,
            y if y < H => Block::DIRT,
            _ => Block::AIR,
        }
    }
}