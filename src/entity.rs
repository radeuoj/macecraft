use glam::{BVec3, IVec3, Vec3, ivec3, vec3};

use crate::{aabb::AABB, world::World};

pub struct Entity {
    pub position: Vec3,
    pub yaw: f32,
    pub pitch: f32,
    pub velocity: Vec3,
    grounded: bool,
    pub flying: bool,
    pub colliding: bool,
    width: f32,
    height: f32,
    world: *mut World,
}

impl Entity {
    pub const UP: Vec3 = Vec3::Y;
    pub const GRAVITY: f32 = 32.0;

    pub fn new(position: Vec3, yaw: f32, pitch: f32, width: f32, height: f32) -> Self {
        Self {
            position,
            yaw,
            pitch,
            velocity: Vec3::ZERO,
            grounded: false,
            flying: false,
            colliding: true,
            width,
            height,
            world: std::ptr::null_mut(),
        }
    }

    pub fn set_world(&mut self, world: *mut World) {
        self.world = world;
    }

    pub fn world_mut(&self) -> &mut World {
        unsafe { &mut *self.world }
    }

    pub fn forward(&self) -> Vec3 {
        vec3(
            self.yaw.cos() * self.pitch.cos(),
            self.pitch.sin(),
            self.yaw.sin() * self.pitch.cos(),
        )
    }

    pub fn horizontal_forward(&self) -> Vec3 {
        vec3(self.yaw.cos(), 0.0, self.yaw.sin())
    }

    pub fn right(&self) -> Vec3 {
        self.horizontal_forward().cross(Self::UP)
    }

    pub fn grounded(&self) -> bool {
        self.grounded
    }

    pub fn tick(&mut self) {
        self.handle_physics();
    }

    /**
     * returns coords of the blocks the entity could collide with
     * assumes the entity is about the size of the player
     */
    fn get_colliders(position: Vec3) -> [IVec3; 36] {
        let mut result = [IVec3::ZERO; _];
        let mut next_index = 0usize;

        for i in -1..=1 {
            for j in -1..=2 {
                for k in -1..=1 {
                    let block_pos = position.floor().as_ivec3() + ivec3(i, j, k);
                    result[next_index] = block_pos;
                    next_index += 1;
                }
            }
        }

        result
    }


    pub fn is_colliding(&self) -> bool {
        let player = AABB::from_player(self.position);

        for block_pos in Self::get_colliders(self.position) {
            let block = AABB::from_block(block_pos);

            if !self.world_mut().is_air(block_pos) && AABB::collision(&player, &block) {
                return true;
            }
        }

        false
    }

    fn handle_physics(&mut self) {
        if !self.flying {
            self.velocity.y -= Self::GRAVITY * World::TICK_DELTA_TIME;
        }

        let delta = self.velocity * World::TICK_DELTA_TIME;
        let moved = self.physics_move(delta);
        let mut stopped = BVec3::FALSE;
        let old_vel_y = self.velocity.y;

        for i in 0..3 {
            if (moved[i] - delta[i]).abs() > 0.0 {
                stopped.set(i, true);
                self.velocity[i] = 0.0;
            }
        }

        self.grounded = stopped.y && old_vel_y < 0.0;
    }

    /**
     * returns how much it was actually moved
     */
    fn physics_move(&mut self, delta: Vec3) -> Vec3 {
        let mut res = Vec3::ZERO;

        for i in 0..3 {
            res[i] = self.physics_move_axis(i, delta[i]);
        }

        if self.colliding && self.is_colliding() {
            log::error!("Entity is colliding after physics move");
        }

        res
    }

    /**
     * returns how much it was actually moved
     */
    fn physics_move_axis(&mut self, axis: usize, delta: f32) -> f32 {
        let mut moved = delta;
        let mut new_pos = self.position;
        new_pos[axis] += moved;
        let player = AABB::from_player(new_pos);
        let mut max_pen = 0.0f32;
        
        for block_pos in Self::get_colliders(new_pos) {
            let block = AABB::from_block(block_pos);

            if !self.world_mut().is_air(block_pos) && AABB::collision(&player, &block) {
                let depth = AABB::collision_depth(&player, &block);
                max_pen = max_pen.max(depth[axis]);
            }   
        }

        if !self.colliding {
            max_pen = 0.0;
        }

        moved -= moved.signum() * max_pen;
        // moved = if moved.abs() <= 0.0001 { 0.0 } else { moved };
        self.position[axis] += moved;

        moved
    }
}