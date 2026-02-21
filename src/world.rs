use std::collections::HashMap;

use glam::Vec3;

use crate::{chunk::{Block, Chunk}, input::Input, player::Player};

pub struct World {
    player: Player,
    chunks: HashMap<glam::IVec3, Chunk>,
}

impl World {
    pub const MAX_CHUNKS: usize = 256;

    pub fn new() -> Self {
        Self {
            player: Player::new(),
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
            None => Block::AIR,
        }
    }

    pub fn set_block(&mut self, pos: glam::IVec3, block: Block) {
        let (chunk_pos, local_pos) = World::world_pos_to_chunk_pos(pos);

        if let Some(chunk) = self.chunks.get_mut(&chunk_pos) {
            chunk.set(local_pos, block);
        } else {
            panic!("Chunk at {:?} was not loaded yet", chunk_pos);
        }
    }

    pub fn is_air(&self, pos: glam::IVec3) -> bool {
        self.get_block(pos) == Block::AIR
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

    pub fn update(&mut self, delta_time: f32, input: &Input) {
        self.player.update(delta_time, input);
        let world = self as *mut World;
        self.player.handle_block_manip(input, world);
    }

    pub fn get_player(&self) -> &Player {
        &self.player
    }

    /**
     * https://lodev.org/cgtutor/raycasting.html
     * https://aaaa.sh/creatures/dda-algorithm-interactive/
     * returns hit position in global coords
     */
    pub fn raycast(&self, mut origin: glam::Vec3, dir: glam::Vec3, max_dist: f32) -> Option<glam::IVec3> {
        assert!(dir.is_normalized());

        let ray_unit_step_size = (1.0 / dir.abs()).map(|e| if e.is_infinite() { 0.0 } else { e }); // note division by zero gives infinity
        let step = dir.signum();

        let mut ray_length = ray_unit_step_size * glam::Vec3::select(
            dir.cmplt(Vec3::ZERO),
            origin.fract(),
            1.0 - origin.fract()
        );

        let mut dist = 0.0;
        while dist <= max_dist {
            let block_pos = origin.floor().as_ivec3();
            if !self.is_air(block_pos) {
                return Some(block_pos)
            }

            if step.x != 0.0 && ray_length.x < ray_length.y && ray_length.x < ray_length.z {
                origin.x += step.x;
                dist = f32::max(dist, ray_length.x);
                ray_length.x += ray_unit_step_size.x;
            } else if step.y != 0.0 && ray_length.y < ray_length.x && ray_length.y < ray_length.z {
                origin.y += step.y;
                dist = f32::max(dist, ray_length.y);
                ray_length.y += ray_unit_step_size.y;
            } else if step.z != 0.0 {
                origin.z += step.z;
                dist = f32::max(dist, ray_length.z);
                ray_length.z += ray_unit_step_size.z;
            }
        }

        None
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_ray_unit_step_size() {
        let dir = glam::vec3(0.5, 0.5, 0.0);
        assert_eq!(1.0 / dir, glam::vec3(2.0, 2.0, f32::INFINITY));
    }
}