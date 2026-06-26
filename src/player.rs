use glam::*;
use winit::{event::MouseButton, keyboard::KeyCode};

use crate::{aabb::AABB, block::{Block, BlockFace}, entity::Entity, input::Input, world::World};

pub struct Player {
    entity: Entity,
    target: Option<(IVec3, BlockFace)>,
    can_place: bool,
    input: Input,
}

impl Player {
    pub const ACCEL: f32 = 30.0;
    pub const SPRINT_ACCEL: f32 = 200.0;
    pub const SENSITIVITY: f32 = 0.001;
    pub const REACH: f32 = 5.0;
    pub const WIDTH: f32 = 0.5;
    pub const HEIGHT: f32 = 1.9;
    pub const EYE_LEVEL: f32 = 1.7;
    pub const DRAG: f32 = 7.0;
    pub const JUMP_FORCE: f32 = 9.0;

    pub fn new() -> Self {
        Self {
            entity: Entity::new(
                vec3(0.0, 20.0, 0.0), 
                -90f32.to_radians(), 
                0.0, 
                Self::WIDTH, 
                Self::HEIGHT
            ),
            target: None,
            can_place: false,
            input: Input::new(),
        }
    }

    pub fn entity(&self) -> &Entity {
        &self.entity
    }

    pub fn entity_mut(&mut self) -> &mut Entity {
        &mut self.entity
    }

    pub fn get_target_pos(&self) -> Option<IVec3> {
        self.target.map(|(pos, _)| pos)
    }

    pub fn get_target_face(&self) -> Option<BlockFace> {
        self.target.map(|(_, face)| face)
    }

    pub fn before_update(&mut self, _delta_time: f32, input: &Input) {
        self.input.extend(&input);
    }

    pub fn tick(&mut self) {
        self.handle_moving();
        self.entity.tick();
        self.handle_block_manip();
        self.input.clear();
    }

    pub fn update(&mut self, input: &Input) {
        self.handle_looking(input);
    }

    fn handle_moving(&mut self) {
        let mut move_dir = Vec3::ZERO;
        let forward = self.entity.horizontal_forward();
        let right = self.entity.right();

        use KeyCode::*;
        if self.input.is_key_pressed(KeyW) { move_dir += forward }
        if self.input.is_key_pressed(KeyS) { move_dir -= forward }
        if self.input.is_key_pressed(KeyD) { move_dir += right }
        if self.input.is_key_pressed(KeyA) { move_dir -= right }
        move_dir = move_dir.normalize_or_zero();

        if self.entity.flying {
            if self.input.is_key_pressed(Space) { move_dir += Entity::UP }
            if self.input.is_key_pressed(ShiftLeft) { move_dir -= Entity::UP }
        }

        let jump_force = (self.entity.grounded() && !self.entity.flying && self.input.is_key_pressed(Space))
            .then_some(Entity::UP * Player::JUMP_FORCE).unwrap_or(Vec3::ZERO);

        let accel = self.input.is_key_pressed(ControlLeft)
            .then_some(Player::SPRINT_ACCEL).unwrap_or(Player::ACCEL);

        let mut drag = -self.entity.velocity * Player::DRAG;
        if !self.entity.flying { drag.y = 0.0 }

        let forces = move_dir * accel + drag;
        self.entity.velocity += forces * World::TICK_DELTA_TIME + jump_force;
    }

    fn handle_looking(&mut self, input: &Input) {
        self.entity.yaw += Self::SENSITIVITY * input.mouse_delta.x;
        self.entity.pitch -= Self::SENSITIVITY * input.mouse_delta.y;

        self.entity.pitch = self.entity.pitch.clamp(-89f32.to_radians(), 89f32.to_radians());

        self.target = self.entity.world_mut().raycast(self.entity.position() + Entity::UP * Player::EYE_LEVEL, self.entity.forward(), Player::REACH);
        self.can_place = self.try_place();
    }

    pub fn handle_block_manip(&self) {
        if self.input.is_mouse_button_just_pressed(MouseButton::Left) && 
                let Some(pos) = self.get_target_pos() {
            self.entity.world_mut().set_block(pos, Block::AIR);
        } else if self.can_place && 
                self.input.is_mouse_button_just_pressed(MouseButton::Right) &&
                let Some((pos, face)) = self.target {
            let pos = Block::get_neighbour(pos, face);
            self.entity.world_mut().set_block(pos, Block::COBBLE);
        }
    }

    fn try_place(&self) -> bool {
        if let Some((pos, face)) = self.target {
            let pos = Block::get_neighbour(pos, face);
            let player = AABB::from_player(self.entity.position);
            let block = AABB::from_block(pos);
            !AABB::collision(&player, &block)
        } else {
            false
        }
    }
}