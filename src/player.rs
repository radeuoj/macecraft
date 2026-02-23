
use winit::{event::MouseButton, keyboard::KeyCode};

use crate::{aabb::AABB, block::{self, Block, BlockFace}, input::Input, world::World};

pub struct Player {
    pub position: glam::Vec3,
    pub yaw: f32,
    pub pitch: f32,
    world: *mut World,
    target: Option<(glam::IVec3, BlockFace)>,
}

impl Player {
    pub const UP: glam::Vec3 = glam::Vec3::Y;
    pub const SPEED: f32 = 2.0;
    pub const SPRINT_SPEED: f32 = 20.0;
    pub const SENSITIVITY: f32 = 0.001;
    pub const REACH: f32 = 5.0;
    pub const WIDTH: f32 = 0.5;
    pub const HEIGHT: f32 = 2.0;
    pub const EYE_LEVEL: f32 = 1.7;

    pub fn new() -> Self {
        Self {
            position: glam::vec3(0.0, 10.0, 0.0),
            yaw: -90f32.to_radians(),
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

    pub fn get_target_pos(&self) -> Option<glam::IVec3> {
        self.target.map(|(pos, _)| pos)
    }

    pub fn get_target_face(&self) -> Option<BlockFace> {
        self.target.map(|(_, face)| face)
    }

    pub fn forward(&self) -> glam::Vec3 {
        glam::vec3(
            self.yaw.cos() * self.pitch.cos(),
            self.pitch.sin(),
            self.yaw.sin() * self.pitch.cos(),
        )
    }

    pub fn right(&self) -> glam::Vec3 {
        self.forward().cross(Self::UP)
    }

    pub fn update(&mut self, delta_time: f32, input: &Input) {
        self.handle_moving(delta_time, input);
        self.handle_looking(input);
        self.handle_block_manip(input);
    }

    fn handle_moving(&mut self, delta_time: f32, input: &Input) {
        let mut move_dir = glam::Vec3::ZERO;
        let forward = self.forward().with_y(0.0).normalize_or_zero();
        let right = self.right();

        use KeyCode::*;
        if input.is_key_pressed(KeyW) { move_dir += forward }
        if input.is_key_pressed(KeyS) { move_dir -= forward }
        if input.is_key_pressed(KeyD) { move_dir += right }
        if input.is_key_pressed(KeyA) { move_dir -= right }
        move_dir = move_dir.normalize_or_zero();

        if input.is_key_pressed(Space) { move_dir.y += 1.0 }
        if input.is_key_pressed(ShiftLeft) { move_dir.y -= 1.0 }

        let speed = if input.is_key_pressed(ControlLeft) {
            Self::SPRINT_SPEED
        } else {
            Self::SPEED
        };

        self.position += move_dir * speed * delta_time;
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
        for i in -1..=1 {
            for j in -1..=2 {
                for k in -1..=1 {
                    let player = AABB::from_player(self.position);
                    let block_pos = self.position.floor().as_ivec3() + glam::ivec3(i, j, k);
                    let block = AABB::from_block(block_pos);

                    if !self.world().is_air(block_pos) && AABB::collision(&player, &block) {
                        return true;
                    }
                }
            }
        }

        false
    }
}