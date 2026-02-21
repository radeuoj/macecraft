
use winit::{event::MouseButton, keyboard::KeyCode};

use crate::{chunk::Block, input::Input, world::World};

pub struct Player {
    pub position: glam::Vec3,
    pub yaw: f32,
    pub pitch: f32,
}

impl Player {
    pub const UP: glam::Vec3 = glam::Vec3::Y;
    pub const SPEED: f32 = 2.0;
    pub const SPRINT_SPEED: f32 = 20.0;
    pub const SENSITIVITY: f32 = 0.001;

    pub fn new() -> Self {
        Self {
            position: glam::Vec3::ZERO,
            yaw: -90f32.to_radians(),
            pitch: 0.0,
        }
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
    }

    // TODO: fix this so it doesnt use raw pointers
    pub fn handle_block_manip(&self, input: &Input, world: *mut World) {
        unsafe {
            if input.is_mouse_button_just_pressed(MouseButton::Left) && 
                    let Some(pos) = (*world).raycast(self.position, self.forward(), 5.0) {
                (*world).set_block(pos, Block::AIR);
            }
        }
    }
}