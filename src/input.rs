use std::collections::HashSet;

use glam::*;
use winit::event::MouseButton;
use winit::keyboard::KeyCode;

pub struct Input {
    pub just_pressed_keys: HashSet<KeyCode>,
    pub active_keys: HashSet<KeyCode>,
    pub mouse_delta: Vec2,
    pub just_pressed_mouse_buttons: HashSet<MouseButton>,
    pub active_mouse_buttons: HashSet<MouseButton>,
}

#[allow(dead_code)]
impl Input {
    pub fn new() -> Self {
        Self {
            just_pressed_keys: HashSet::new(),
            active_keys: HashSet::new(),
            mouse_delta: Vec2::ZERO,
            just_pressed_mouse_buttons: HashSet::new(),
            active_mouse_buttons: HashSet::new(),
        }
    }

    pub fn is_key_pressed(&self, key: KeyCode) -> bool {
        self.active_keys.contains(&key)
    }

    pub fn is_key_just_pressed(&self, key: KeyCode) -> bool {
        self.just_pressed_keys.contains(&key)
    }

    pub fn is_mouse_button_pressed(&self, button: MouseButton) -> bool {
        self.active_mouse_buttons.contains(&button)
    }

    pub fn is_mouse_button_just_pressed(&self, button: MouseButton) -> bool {
        self.just_pressed_mouse_buttons.contains(&button)
    }
}