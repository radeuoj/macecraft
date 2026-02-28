use std::{collections::{HashMap, HashSet}, mem};

use glam::*;

use crate::{block::{Block, BlockFace}, chunk::Chunk, input::Input, player::Player};

pub struct World {
    player: Player,
    chunks: HashMap<IVec3, Chunk>,
    dirty_chunks: HashSet<IVec3>,
}

impl World {
    pub const MAX_CHUNKS: usize = 256;
    pub const RENDER_DISTANCE: i32 = 3;

    pub fn new(player: Player) -> Self {
        Self {
            player,
            chunks: HashMap::new(),
            dirty_chunks: HashSet::new(),
        }
    }

    pub fn add_chunk(&mut self, pos: IVec3, mut chunk: Chunk) {
        match pos.y {
            0 => chunk.generate_superflat(),
            y if y < 0 => chunk.generate_fill(Block::DIRT),
            _ => (),
        }

        self.chunks.insert(pos, chunk);
        self.mark_very_dirty(pos);
    }

    pub fn get_chunk(&self, pos: IVec3) -> Option<&Chunk> {
        self.chunks.get(&pos)
    }

    pub fn chunk_count(&self) -> usize {
        self.chunks.len()
    }

    pub fn get_block(&self, pos: IVec3) -> Block {
        let (chunk_pos, local_pos) = World::world_pos_to_chunk_pos(pos);

        match self.chunks.get(&chunk_pos) {
            Some(chunk) => chunk.get(local_pos),
            None => Block::AIR,
        }
    }

    pub fn player(&self) -> &Player {
        &self.player
    }

    pub fn player_mut(&mut self) -> &mut Player {
        &mut self.player
    }

    fn mark_dirty(&mut self, pos: IVec3) {
        if self.chunks.contains_key(&pos) { 
            self.dirty_chunks.insert(pos);
        }
    }

    fn mark_very_dirty(&mut self, pos: IVec3) {
        self.mark_dirty(pos);

        for neighbour in Chunk::get_neighbours(pos) {
            self.mark_dirty(neighbour);
        }
    }

    pub fn dirty_chunks(&mut self) -> HashSet<IVec3> {
        mem::replace(&mut self.dirty_chunks, HashSet::new())
    }

    pub fn set_block(&mut self, pos: IVec3, block: Block) {
        let (chunk_pos, local_pos) = World::world_pos_to_chunk_pos(pos);

        if let Some(chunk) = self.chunks.get_mut(&chunk_pos) {
            chunk.set(local_pos, block);
            self.mark_very_dirty(chunk_pos);
        } else {
            panic!("Chunk at {:?} was not loaded yet", chunk_pos);
        }
    }

    pub fn is_air(&self, pos: IVec3) -> bool {
        self.get_block(pos) == Block::AIR
    }

    /**
     * returns (chunk_pos, local_pos)
     */
    pub fn world_pos_to_chunk_pos(pos: IVec3) -> (IVec3, UVec3) {
        let mut chunk_pos = pos / Chunk::SIZE as i32;
        let mut local_pos = pos % Chunk::SIZE as i32;
        let is_neg = local_pos.cmplt(IVec3::ZERO);
        chunk_pos -= IVec3::select(is_neg, IVec3::ONE, IVec3::ZERO);
        local_pos += IVec3::select(is_neg, IVec3::ONE * Chunk::SIZE as i32, IVec3::ZERO);
        (chunk_pos, local_pos.as_uvec3())
    }

    pub fn chunk_pos_to_world_pos(chunk_pos: IVec3, local_pos: UVec3) -> IVec3 {
        chunk_pos * Chunk::SIZE as i32 + local_pos.as_ivec3()
    }

    pub fn update(&mut self, delta_time: f32, input: &Input) {
        self.player.update(delta_time, input);
        self.render_new_chunks();
    }

    /**
     * https://lodev.org/cgtutor/raycasting.html
     * https://aaaa.sh/creatures/dda-algorithm-interactive/
     * returns hit position in global coords
     */
    pub fn raycast(&self, mut origin: Vec3, dir: Vec3, max_dist: f32) -> Option<(IVec3, BlockFace)> {
        assert!(dir.is_normalized());

        let ray_unit_step_size = 1.0 / dir.abs(); // note division by zero gives infinity
        let step = dir.signum();

        let mut ray_length = ray_unit_step_size * Vec3::select(
            dir.cmplt(Vec3::ZERO),
            origin - origin.floor(),
            1.0 - origin + origin.floor()
        );

        let mut dist = 0.0;
        let mut block_face = BlockFace::YP;
        while dist <= max_dist {
            let block_pos = origin.floor().as_ivec3();
            if !self.is_air(block_pos) {
                return Some((block_pos, block_face))
            }

            if step.x != 0.0 && ray_length.x < ray_length.y && ray_length.x < ray_length.z {
                origin.x += step.x;
                dist = f32::max(dist, ray_length.x);
                ray_length.x += ray_unit_step_size.x;
                block_face = if step.x > 0.0 { BlockFace::XP } else { BlockFace::XN };
            } else if step.y != 0.0 && ray_length.y < ray_length.x && ray_length.y < ray_length.z {
                origin.y += step.y;
                dist = f32::max(dist, ray_length.y);
                ray_length.y += ray_unit_step_size.y;
                block_face = if step.y > 0.0 { BlockFace::YP } else { BlockFace::YN };
            } else if step.z != 0.0 {
                origin.z += step.z;
                dist = f32::max(dist, ray_length.z);
                ray_length.z += ray_unit_step_size.z;
                block_face = if step.z > 0.0 { BlockFace::ZP } else { BlockFace::ZN };
            }
        }

        None
    }

    fn render_new_chunks(&mut self) {
        let (origin, _) = World::world_pos_to_chunk_pos(self.player.position().as_ivec3());
        let dist = World::RENDER_DISTANCE - 1;

        for i in -dist..=dist {
            for j in -dist..=dist {
                for k in -dist..=dist {
                    let pos = origin + ivec3(i, j, k);
                    if self.chunks.contains_key(&pos) { continue }

                    self.add_chunk(pos, Chunk::new());
                }
            }
        }
    }
}