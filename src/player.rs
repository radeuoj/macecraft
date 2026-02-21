use std::collections::HashSet;

use winit::keyboard::KeyCode;

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

    pub fn update(&mut self, delta_time: f32, active_keys: &HashSet<KeyCode>, mouse_delta: glam::Vec2) {
        self.handle_moving(delta_time, active_keys);
        self.handle_looking(mouse_delta);
    }

    fn handle_moving(&mut self, delta_time: f32, active_keys: &HashSet<KeyCode>) {
        let mut move_dir = glam::Vec3::ZERO;
        let forward = self.forward().with_y(0.0).normalize_or_zero();
        let right = self.right();

        use KeyCode::*;
        if active_keys.contains(&KeyW) { move_dir += forward }
        if active_keys.contains(&KeyS) { move_dir -= forward }
        if active_keys.contains(&KeyD) { move_dir += right }
        if active_keys.contains(&KeyA) { move_dir -= right }
        move_dir = move_dir.normalize_or_zero();

        if active_keys.contains(&Space) { move_dir.y += 1.0 }
        if active_keys.contains(&ShiftLeft) { move_dir.y -= 1.0 }

        let speed = if active_keys.contains(&ControlLeft) {
            Self::SPRINT_SPEED
        } else {
            Self::SPEED
        };

        self.position += move_dir * speed * delta_time;
    }

    fn handle_looking(&mut self, mouse_delta: glam::Vec2) {
        self.yaw += Self::SENSITIVITY * mouse_delta.x;
        self.pitch -= Self::SENSITIVITY * mouse_delta.y;

        self.pitch = self.pitch.clamp(-89f32.to_radians(), 89f32.to_radians());
    }
}