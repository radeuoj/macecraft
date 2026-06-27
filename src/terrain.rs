use glam::*;
use noise::NoiseFn;

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

pub struct NiceTerrainGen {
    perlin: noise::Perlin,
}

impl NiceTerrainGen {
    pub fn new() -> Self {
        Self { 
            perlin: noise::Perlin::new(123456789),
        }
    }
}

impl TerrainGen for NiceTerrainGen {
    fn get(&self, position: IVec3) -> Block {
        let coords = position.xz().as_dvec2() / 32.0;
        let h = (self.perlin.get(coords.into()) * 10.0) as i32;

        if position.y == h && h >= -1 { Block::GRASS }
        else if position.y < h || (h < -1 && position.y == h) { Block::DIRT }
        else { Block::AIR }
    }
}