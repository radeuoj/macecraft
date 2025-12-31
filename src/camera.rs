use std::collections::HashSet;
use winit::keyboard::KeyCode;

pub struct Camera {
    position: glam::Vec3,
    yaw: f32,
    pitch: f32,
    proj: glam::Mat4,
}

impl Camera {
    pub const UP: glam::Vec3 = glam::Vec3::Y;
    pub const FOV_Y: f32 = 60.0;
    pub const Z_NEAR: f32 = 0.1;
    pub const Z_FAR: f32 = 1000.0;
    pub const SPEED: f32 = 2.0;
    pub const SENSITIVITY: f32 = 0.002;

    pub fn new(aspect_ratio: f32) -> Self {
        Self {
            position: glam::Vec3::ZERO,
            yaw: -90f32.to_radians(),
            pitch: 0.0,
            proj: Self::build_proj_matrix(aspect_ratio),
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

    pub fn set_aspect_ratio(&mut self, aspect_ratio: f32) {
        self.proj = Self::build_proj_matrix(aspect_ratio);
    }

    fn build_proj_matrix(aspect_ratio: f32) -> glam::Mat4 {
        glam::Mat4::perspective_rh(
            Self::FOV_Y.to_radians(),
            aspect_ratio,
            Self::Z_NEAR,
            Self::Z_FAR,
        )
    }

    pub fn build_view_proj_matrix(&self) -> glam::Mat4 {
        let view = glam::Mat4::look_at_rh(
            self.position,
            self.position + self.forward(),
            Self::UP,
        );

        self.proj * view
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

        self.position += move_dir * Self::SPEED * delta_time;
    }

    fn handle_looking(&mut self, mouse_delta: glam::Vec2) {
        self.yaw += Self::SENSITIVITY * mouse_delta.x;
        self.pitch -= Self::SENSITIVITY * mouse_delta.y;

        self.pitch = self.pitch.clamp(-89f32.to_radians(), 89f32.to_radians());
    }
}