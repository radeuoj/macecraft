mod texture;
mod camera;
mod renderer;
mod chunk;
mod world;
mod imgui_renderer;
mod player;
mod input;
mod block;
mod aabb;

use crate::camera::Camera;
use crate::input::Input;
use crate::player::Player;
use crate::renderer::Renderer;
use crate::world::World;
use std::sync::Arc;
use std::time::Instant;
use glam::*;
use winit::application::ApplicationHandler;
use winit::dpi::PhysicalPosition;
use winit::event::{DeviceEvent, DeviceId, MouseButton, RawKeyEvent, WindowEvent};
use winit::event_loop::ActiveEventLoop;
use winit::keyboard::{KeyCode, PhysicalKey};
use winit::window::{CursorGrabMode, Window, WindowId};

struct State {
    window: Arc<Window>,
    size: winit::dpi::PhysicalSize<u32>,
    camera: Camera,
    renderer: Renderer,
    input: Input,
    is_mouse_captured: bool,

    world: World,
}

impl State {
    async fn new(window: Arc<Window>) -> State {
        let size = window.inner_size();
        let camera = Camera::new(size.width as f32 / size.height as f32);
        let renderer = pollster::block_on(Renderer::new(window.clone(), &camera));

        let player = Player::new();
        let world = World::new(player);

        Self {
            window,
            size,
            camera,
            renderer,
            input: Input::new(),
            is_mouse_captured: false,
            world,
        }
    }

    fn resize(&mut self, size: winit::dpi::PhysicalSize<u32>) {
        if size.width == 0 || size.height == 0 { return; }
        self.size = size;
        self.camera.set_aspect_ratio(size.width as f32 / size.height as f32);
        self.renderer.resize(size);
    }

    fn toggle_mouse_capture(&mut self) {
        self.is_mouse_captured = !self.is_mouse_captured;

        if self.is_mouse_captured {
            self.window
                .set_cursor_grab(CursorGrabMode::Confined)
                .or_else(|_| self.window.set_cursor_grab(CursorGrabMode::Locked))
                .unwrap();
            self.window.set_cursor_visible(false);
        } else {
            self.window.set_cursor_grab(CursorGrabMode::None).unwrap();
            self.window.set_cursor_visible(true);
        }
    }

    fn handle_key(&mut self, code: KeyCode, is_pressed: bool) {
        if is_pressed {
            self.input.active_keys.insert(code);
            self.input.just_pressed_keys.insert(code);
            if code == KeyCode::AltLeft { self.toggle_mouse_capture() }
        } else {
            self.input.active_keys.remove(&code);
        }
    }

    fn handle_mouse(&mut self, delta: Vec2) {
        if !self.is_mouse_captured { return }
        self.input.mouse_delta += delta;
    }

    fn handle_mouse_button(&mut self, button: MouseButton, is_pressed: bool) {
        if is_pressed {
            self.input.active_mouse_buttons.insert(button);
            self.input.just_pressed_mouse_buttons.insert(button);
        } else {
            self.input.active_mouse_buttons.remove(&button);
        }
    }

    fn update(&mut self, delta_time: f32) {
        self.world.update(delta_time, &self.input);
        self.camera.update_from_player(self.world.player());
        self.renderer.update_camera(&self.camera);

        self.renderer.update_target_block(self.world.player().get_target_pos());

        self.update_imgui(delta_time);
    }

    fn update_imgui(&mut self, delta_time: f32) {
        let pos = self.camera.position;
        let yaw = self.camera.yaw;
        let pitch = self.camera.pitch;

        let player = self.world.player_mut();
        let collision = player.is_colliding();
        let flying = player.flying_mut() as *mut bool;

        let target_pos = player.get_target_pos();
        let target_face = player.get_target_face();
        let target_block = target_pos.map(|pos| self.world.get_block(pos));

        let chunk_count = self.world.chunk_count();

        self.renderer.update_imgui(move |ui| {
            ui.text(format!("FPS: {}", 1.0 / delta_time));
            ui.text(format!("Position: {:+.2} {:+.2} {:+.2}", pos.x, pos.y, pos.z));
            ui.text(format!("Yaw: {:+.2}", yaw));
            ui.text(format!("Pitch: {:+.2}", pitch));
            ui.text(format!("Target: {:?}", target_pos));
            ui.text(format!("Target face: {:?}", target_face));
            ui.text(format!("Target block: {:?}", target_block));
            ui.text(format!("Colision: {}", collision));
            ui.text(format!("Chunk count: {}", chunk_count));

            unsafe {
                ui.checkbox("Flying", &mut *flying);
            }

        });
    }

    fn render(&mut self) -> anyhow::Result<()> {
        for pos in self.world.dirty_chunks() {
            self.renderer.render_chunk(pos, &self.world);
        }

        self.renderer.draw(&self.window)?;

        Ok(())
    }
}

pub struct App {
    state: Option<State>,
    delta_time: Instant,
}

impl App {
    pub fn new() -> Self {
        Self {
            state: None,
            delta_time: Instant::now(),
        }
    }

    fn handle_key(&mut self, event_loop: &ActiveEventLoop, code: KeyCode, is_pressed: bool) {
        if is_pressed && code == KeyCode::Escape {
            event_loop.exit();
        }

        let state = self.state.as_mut().unwrap();
        state.handle_key(code, is_pressed);
    }

    fn handle_redraw(&mut self)  {
        let delta_time = self.delta_time.elapsed();
        self.delta_time = Instant::now();

        let state = self.state.as_mut().unwrap();
        state.update(delta_time.as_secs_f32());

        match state.render() {
            Ok(_) => (),
            Err(e) => log::error!("Render error: {}", e),
        }

        state.input.mouse_delta = Vec2::ZERO;
        state.input.just_pressed_keys.clear();
        state.input.just_pressed_mouse_buttons.clear();

        if state.is_mouse_captured {
            state.window.set_cursor_position(PhysicalPosition::new(
                state.size.width / 2, state.size.height / 2)).unwrap();
        }

        state.window.request_redraw();
    }

    fn state(&mut self) -> &mut State {
        self.state.as_mut().unwrap()
    }
}

impl ApplicationHandler for App {
    fn resumed(&mut self, event_loop: &ActiveEventLoop) {
        let window_attributes = Window::default_attributes()
            .with_title("Macecraft")
            .with_inner_size(winit::dpi::PhysicalSize::new(1600, 900));
        let window = Arc::new(event_loop.create_window(window_attributes).unwrap());

        self.state = Some(pollster::block_on(State::new(window)));
        let world = &mut self.state().world as _;
        self.state().world.player_mut().set_world(world);
    }

    fn window_event(&mut self, event_loop: &ActiveEventLoop, _window_id: WindowId, event: WindowEvent) {
        match event {
            WindowEvent::CloseRequested => event_loop.exit(),
            WindowEvent::MouseInput { 
                button, 
                state: button_state,
                .. 
            } => self.state().handle_mouse_button(button, button_state.is_pressed()),
            WindowEvent::RedrawRequested => self.handle_redraw(),
            WindowEvent::Resized(size) => self.state().resize(size),
            _ => (),
        }

        let state = self.state();
        state.renderer.handle_imgui_window_event(&state.window, &event);
    }

    fn device_event(&mut self, event_loop: &ActiveEventLoop, _device_id: DeviceId, event: DeviceEvent) {
        let state = self.state.as_mut().unwrap();

        match event {
            DeviceEvent::MouseMotion { delta }
                => state.handle_mouse(DVec2::from(delta).as_vec2()),
            DeviceEvent::Key(RawKeyEvent { 
                physical_key: PhysicalKey::Code(code), 
                state: key_state 
            }) => self.handle_key(event_loop, code, key_state.is_pressed()),
            _ => (),
        }
    }
}