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

    /**
     * this clears the just pressed keys and mouse buttons and should
     * be called at the end of the frame
     */
    pub fn update(&mut self) {
        self.mouse_delta = Vec2::ZERO;
        self.just_pressed_keys.clear();
        self.just_pressed_mouse_buttons.clear();
    }

    pub fn clear(&mut self) {
        self.just_pressed_keys.clear();
        self.active_keys.clear();
        self.mouse_delta = Vec2::ZERO;
        self.just_pressed_mouse_buttons.clear();
        self.active_mouse_buttons.clear();
    }

    pub fn extend(&mut self, other: &Self) {
        self.just_pressed_keys.extend(&other.just_pressed_keys);
        self.active_keys.extend(&other.active_keys);
        self.mouse_delta += other.mouse_delta;
        self.just_pressed_mouse_buttons.extend(&other.just_pressed_mouse_buttons);
        self.active_mouse_buttons.extend(&other.active_mouse_buttons);
    }
}