use std::{collections::{HashMap, HashSet}, mem, time::{Duration, Instant}};

use glam::*;

use crate::{block::{Block, BlockFace}, chunk::Chunk, input::Input, player::Player, terrain::TerrainGen};

pub struct World {
    player: Player,
    chunks: HashMap<IVec3, Chunk>,
    dirty_chunks: HashSet<IVec3>,
    terrain_gen: Box<dyn TerrainGen>,
    last_tick: Instant,
    last_frame: Instant,
}

impl World {
    pub const MAX_CHUNKS: usize = 256;
    pub const RENDER_DISTANCE: i32 = 8;
    pub const TPS: i32 = 50;
    pub const TICK_DELTA_TIME: f32 = 1.0 / Self::TPS as f32;
    pub const TICK_DURATION: Duration = Duration::from_millis(1000 / Self::TPS as u64);

    pub fn new(player: Player, terrain_gen: impl TerrainGen + 'static) -> Self {
        Self {
            player,
            chunks: HashMap::new(),
            dirty_chunks: HashSet::new(),
            terrain_gen: Box::new(terrain_gen),
            last_tick: Instant::now(),
            last_frame: Instant::now(),
        }
    }

    pub fn add_chunk(&mut self, pos: IVec3, mut chunk: Chunk) {
        self.generate_chunk(pos, &mut chunk);
        self.chunks.insert(pos, chunk);
        self.mark_very_dirty(pos);
    }

    pub fn get_chunk(&self, pos: IVec3) -> Option<&Chunk> {
        self.chunks.get(&pos)
    }

    pub fn chunk_count(&self) -> usize {
        self.chunks.len()
    }

    fn generate_chunk(&self, chunk_pos: IVec3, chunk: &mut Chunk) {
        for x in 0..Chunk::SIZE {
            for y in 0..Chunk::SIZE {
                for z in 0..Chunk::SIZE {
                    let local_pos = uvec3(x, y, z);
                    let global_pos = World::chunk_pos_to_world_pos(chunk_pos, local_pos);

                    unsafe {
                        chunk.set_unchecked(local_pos, self.terrain_gen.get(global_pos));
                    }
                }
            }
        }
    }

    pub fn get_block(&self, pos: IVec3) -> Block {
        let (chunk_pos, local_pos) = World::world_pos_to_chunk_pos(pos);

        match self.chunks.get(&chunk_pos) {
            Some(chunk) => unsafe { chunk.get_unchecked(local_pos) },
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

    /**
     * also marks the neighbours dirty
     */
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
            unsafe { chunk.set_unchecked(local_pos, block) }
            self.mark_dirty(chunk_pos);
            self.mark_dirty_if_pos_on_edge(chunk_pos, local_pos);
        } else {
            panic!("Chunk at {:?} was not loaded yet", chunk_pos);
        }
    }

    pub fn is_solid(&self, pos: IVec3) -> bool {
        match self.get_block(pos) {
            Block::AIR | Block::WATER => false,
            _ => true,
        }
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
        self.last_frame = Instant::now();
        self.player.before_update(delta_time, input);
        
        while self.last_frame - self.last_tick >= Self::TICK_DURATION {
            self.last_tick += Self::TICK_DURATION;
            self.tick();
        }

        self.player.update(input);
        self.render_new_chunks();
    }

    pub fn tick(&mut self) {
        self.player.tick();
    }

    pub fn get_tick_alpha(&self) -> f32 {
        ((self.last_frame - self.last_tick).as_secs_f32() / Self::TICK_DURATION.as_secs_f32())
            .clamp(0.0, 1.0)
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
            if self.is_solid(block_pos) {
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

    /**
     * returns when it finds an ungenerated chunk
     */
    fn render_new_chunks(&mut self) {
        let (origin, _) = World::world_pos_to_chunk_pos(self.player.entity_mut().position.as_ivec3());
        let dist = World::RENDER_DISTANCE - 1;

        for i in 0..=dist {
            for j in -i..=i {
                for k in -i..=i {
                    let pos = origin + ivec3(-i, j, k);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }

                    let pos = origin + ivec3(i, j, k);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }

                    let pos = origin + ivec3(j, -i, k);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }

                    let pos = origin + ivec3(j, i, k);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }

                    let pos = origin + ivec3(j,k, -i);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }

                    let pos = origin + ivec3(j, k, i);
                    if !self.chunks.contains_key(&pos) {
                        self.add_chunk(pos, Chunk::new());
                        return;
                    }
                }
            }
        }
    }

    fn mark_dirty_if_pos_on_edge(&mut self, chunk_pos: IVec3, local_pos: UVec3) {
        const MAX: u32 = Chunk::SIZE - 1;

        for i in 0..3 {
            let mut delta = IVec3::ZERO;
            delta[i] = 1;

            match local_pos[i] {
                0 => self.mark_dirty(chunk_pos - delta),
                MAX => self.mark_dirty(chunk_pos + delta),
                _ => (),
            }
        }
    }
}