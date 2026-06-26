use glam::*;
use crate::player::Player;

pub struct Camera {
    pub position: Vec3,
    pub yaw: f32,
    pub pitch: f32,
    proj: Mat4,
}

impl Camera {
    pub const UP: Vec3 = Vec3::Y;
    pub const FOV_Y: f32 = 60.0;
    pub const Z_NEAR: f32 = 0.1;
    pub const Z_FAR: f32 = 1000.0;

    pub fn new(aspect_ratio: f32) -> Self {
        Self {
            position: Vec3::ZERO,
            yaw: -90f32.to_radians(),
            pitch: 0.0,
            proj: Self::build_proj_matrix(aspect_ratio),
        }
    }

    pub fn forward(&self) -> Vec3 {
        vec3(
            self.yaw.cos() * self.pitch.cos(),
            self.pitch.sin(),
            self.yaw.sin() * self.pitch.cos(),
        )
    }

    pub fn set_aspect_ratio(&mut self, aspect_ratio: f32) {
        self.proj = Self::build_proj_matrix(aspect_ratio);
    }

    pub fn update_from_player(&mut self, player: &Player) {
        self.position = player.entity().position() + Camera::UP * Player::EYE_LEVEL;
        self.yaw = player.entity().yaw;
        self.pitch = player.entity().pitch;
    }

    fn build_proj_matrix(aspect_ratio: f32) -> Mat4 {
        Mat4::perspective_rh(
            Self::FOV_Y.to_radians(),
            aspect_ratio,
            Self::Z_NEAR,
            Self::Z_FAR,
        )
    }

    pub fn build_view_proj_matrix(&self) -> Mat4 {
        let view = Mat4::look_at_rh(
            self.position,
            self.position + self.forward(),
            Self::UP,
        );

        self.proj * view
    }
}