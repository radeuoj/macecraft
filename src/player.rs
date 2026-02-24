use glam::*;
use winit::{event::MouseButton, keyboard::KeyCode};

use crate::{aabb::AABB, block::{Block, BlockFace}, input::Input, world::World};

pub struct Player {
    position: Vec3,
    yaw: f32,
    pitch: f32,
    velocity: Vec3,
    world: *mut World,
    target: Option<(IVec3, BlockFace)>,
}

impl Player {
    pub const UP: Vec3 = Vec3::Y;
    pub const ACCEL: f32 = 20.0;
    pub const SPRINT_ACCEL: f32 = 200.0;
    pub const SENSITIVITY: f32 = 0.001;
    pub const REACH: f32 = 5.0;
    pub const WIDTH: f32 = 0.5;
    pub const HEIGHT: f32 = 2.0;
    pub const EYE_LEVEL: f32 = 1.7;
    pub const DRAG: f32 = 5.0;
    pub const GRAVITY: f32 = 18.0;
    pub const JUMP_FORCE: f32 = 8.0;
    pub const PHYSICS_EPSILON: f32 = 0.01;

    pub fn new() -> Self {
        Self {
            position: vec3(0.0, 10.0, 0.0),
            yaw: -90f32.to_radians(),
            velocity: Vec3::ZERO,
            pitch: 0.0,
            world: std::ptr::null_mut(),
            target: None,
        }
    }

    pub fn set_world(&mut self, world: *mut World) {
        self.world = world;
    }

    fn world(&self) -> &mut World {
        unsafe { &mut *self.world }
    }

    pub fn get_target_pos(&self) -> Option<IVec3> {
        self.target.map(|(pos, _)| pos)
    }

    pub fn get_target_face(&self) -> Option<BlockFace> {
        self.target.map(|(_, face)| face)
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

    pub fn position(&self) -> Vec3 {
        self.position
    }

    pub fn yaw(&self) -> f32 {
        self.yaw
    }

    pub fn pitch(&self) -> f32 {
        self.pitch
    }

    pub fn update(&mut self, delta_time: f32, input: &Input) {
        self.handle_moving(delta_time, input);
        self.handle_physics(delta_time);
        self.handle_looking(input);
        self.handle_block_manip(input);
    }

    fn handle_moving(&mut self, delta_time: f32, input: &Input) {
        let mut move_dir = Vec3::ZERO;
        let forward = self.horizontal_forward();
        let right = self.right();

        use KeyCode::*;
        if input.is_key_pressed(KeyW) { move_dir += forward }
        if input.is_key_pressed(KeyS) { move_dir -= forward }
        if input.is_key_pressed(KeyD) { move_dir += right }
        if input.is_key_pressed(KeyA) { move_dir -= right }
        move_dir = move_dir.normalize_or_zero();

        // if input.is_key_pressed(Space) { move_dir += Player::UP }
        // if input.is_key_pressed(ShiftLeft) { move_dir -= Player::UP }
        let jump_force = if input.is_key_just_pressed(Space) {
            Player::UP * Player::JUMP_FORCE
        } else {
            Vec3::ZERO
        };

        let accel = if input.is_key_pressed(ControlLeft) {
            Player::SPRINT_ACCEL
        } else {
            Player::ACCEL
        };

        let drag = -self.velocity.with_y(0.0) * Player::DRAG;
        let forces = move_dir * accel + drag;
        self.velocity += forces * delta_time + jump_force;
    }

    fn handle_physics(&mut self, delta_time: f32) {
        self.velocity.y -= Player::GRAVITY * delta_time;
        self.physics_move(self.velocity * delta_time);
    }

    fn physics_move(&mut self, delta: Vec3) {
        for i in 0..3 {
            self.physics_move_axis(i, delta[i]);
        }
    }

    fn physics_move_axis(&mut self, axis: usize, delta: f32) {
        let mut moved = delta;
        let mut new_pos = self.position;
        new_pos[axis] += moved;
        let player = AABB::from_player(new_pos);
        let mut max_pen = 0.0f32;
        
        for block_pos in self.get_colliders() {
            let block = AABB::from_block(block_pos);

            if !self.world().is_air(block_pos) && AABB::collision(&player, &block) {
                let depth = AABB::collision_depth(&player, &block);
                max_pen = max_pen.max(depth[axis]);
            }   
        }

        if max_pen > 0.0 {
            self.velocity[axis] = 0.0;
        }

        moved -= moved.signum() * max_pen;
        // moved = if moved.abs() <= Player::PHYSICS_EPSILON { 0.0 } else { moved };
        self.position[axis] += moved;
    }

    fn handle_looking(&mut self, input: &Input) {
        self.yaw += Self::SENSITIVITY * input.mouse_delta.x;
        self.pitch -= Self::SENSITIVITY * input.mouse_delta.y;

        self.pitch = self.pitch.clamp(-89f32.to_radians(), 89f32.to_radians());

        self.target = self.world().raycast(self.position + Player::UP * Player::EYE_LEVEL, self.forward(), Player::REACH);
    }

    pub fn handle_block_manip(&self, input: &Input) {
        if input.is_mouse_button_just_pressed(MouseButton::Left) && 
                let Some(pos) = self.get_target_pos() {
            self.world().set_block(pos, Block::AIR);
        } else if input.is_mouse_button_just_pressed(MouseButton::Right) &&
                let Some((mut pos, face)) = self.target {
            match face {
                BlockFace::ZN => pos.z += 1,
                BlockFace::ZP => pos.z -= 1,
                BlockFace::XN => pos.x += 1,
                BlockFace::XP => pos.x -= 1,
                BlockFace::YN => pos.y += 1,
                BlockFace::YP => pos.y -= 1,
            }

            self.world().set_block(pos, Block::COBBLE);
        }
    }

    pub fn is_colliding(&self) -> bool {
        let player = AABB::from_player(self.position);

        for block_pos in self.get_colliders() {
            let block = AABB::from_block(block_pos);

            if !self.world().is_air(block_pos) && AABB::collision(&player, &block) {
                return true;
            }
        }

        false
    }

    fn get_colliders(&self) -> [IVec3; 36] {
        let mut result = [IVec3::ZERO; _];
        let mut next_index = 0usize;

        for i in -1..=1 {
            for j in -1..=2 {
                for k in -1..=1 {
                    let block_pos = self.position.floor().as_ivec3() + ivec3(i, j, k);
                    result[next_index] = block_pos;
                    next_index += 1;
                }
            }
        }

        result
    }
}