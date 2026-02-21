use std::collections::HashSet;

use winit::event::MouseButton;
use winit::keyboard::KeyCode;

pub struct Input {
    pub active_keys: HashSet<KeyCode>,
    pub mouse_delta: glam::Vec2,
    pub just_pressed_mouse_buttons: HashSet<MouseButton>,
    pub active_mouse_buttons: HashSet<MouseButton>,
}

impl Input {
    pub fn new() -> Self {
        Self {
            active_keys: HashSet::new(),
            mouse_delta: glam::Vec2::ZERO,
            just_pressed_mouse_buttons: HashSet::new(),
            active_mouse_buttons: HashSet::new(),
        }
    }

    pub fn is_key_pressed(&self, key: KeyCode) -> bool {
        self.active_keys.contains(&key)
    }

    pub fn is_mouse_button_pressed(&self, button: MouseButton) -> bool {
        self.active_mouse_buttons.contains(&button)
    }

    pub fn is_mouse_button_just_pressed(&self, button: MouseButton) -> bool {
        self.just_pressed_mouse_buttons.contains(&button)
    }
}