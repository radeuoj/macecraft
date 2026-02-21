mod texture;
mod camera;
mod renderer;
mod chunk;
mod world;
mod imgui_renderer;
mod player;

use crate::camera::Camera;
use crate::renderer::Renderer;
use crate::world::World;
use std::collections::HashSet;
use std::sync::Arc;
use std::time::Instant;
use winit::application::ApplicationHandler;
use winit::event::{DeviceEvent, DeviceId, KeyEvent, WindowEvent};
use winit::event_loop::ActiveEventLoop;
use winit::keyboard::{KeyCode, PhysicalKey};
use winit::window::{CursorGrabMode, Window, WindowId};
use crate::chunk::Chunk;

struct State {
    window: Arc<Window>,
    size: winit::dpi::PhysicalSize<u32>,
    camera: Camera,
    renderer: Renderer,
    active_keys: HashSet<KeyCode>,
    mouse_delta: glam::Vec2,
    is_mouse_captured: bool,

    world: World,
}

impl State {
    async fn new(window: Arc<Window>) -> State {
        let size = window.inner_size();
        let camera = Camera::new(size.width as f32 / size.height as f32);
        let mut renderer = pollster::block_on(Renderer::new(window.clone(), &camera));

        let mut world = World::new();
        for i in -1..=1 {
            for j in -1..=1 {
                let mut chunk = Chunk::new();
                chunk.generate_superflat();
                let pos = (i, 0, j).into();
                world.add_chunk(pos, chunk);
            }
        }

        for i in -1..=1 {
            for j in -1..=1 {
                let pos = (i, 0, j).into();
                renderer.render_chunk(pos, &world);
            }
        }

        Self {
            window,
            size,
            camera,
            renderer,
            active_keys: HashSet::new(),
            mouse_delta: glam::Vec2::ZERO,
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
            self.active_keys.insert(code);
            if code == KeyCode::AltLeft { self.toggle_mouse_capture() }
        } else {
            self.active_keys.remove(&code);
        }
    }

    fn handle_mouse(&mut self, delta: glam::Vec2) {
        if !self.is_mouse_captured { return }
        self.mouse_delta += delta;
    }

    fn update(&mut self, delta_time: f32) {
        self.world.update(delta_time, &self.active_keys, self.mouse_delta);
        self.camera.update_from_player(self.world.get_player());
        self.renderer.update_camera(&self.camera);
        self.update_imgui(delta_time);
    }

    fn update_imgui(&mut self, delta_time: f32) {
        let pos = self.camera.position;
        let yaw = self.camera.yaw;
        let pitch = self.camera.pitch;
        self.renderer.update_imgui(move |ui| {
            ui.text(format!("FPS: {}", 1.0 / delta_time));
            ui.text(format!("Position: {:+.2} {:+.2} {:+.2}", pos.x, pos.y, pos.z));
            ui.text(format!("Yaw: {:+.2}", yaw));
            ui.text(format!("Pitch: {:+.2}", pitch));
        });
    }

    fn render(&mut self) -> anyhow::Result<()> {
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

        state.mouse_delta = glam::Vec2::ZERO;
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
    }

    fn window_event(&mut self, event_loop: &ActiveEventLoop, _window_id: WindowId, event: WindowEvent) {
        match event {
            WindowEvent::CloseRequested => event_loop.exit(),
            WindowEvent::KeyboardInput {
                event: KeyEvent {
                    physical_key: PhysicalKey::Code(code),
                    state: key_state,
                    ..
                },
                ..
            } => self.handle_key(event_loop, code, key_state.is_pressed()),
            WindowEvent::RedrawRequested => self.handle_redraw(),
            WindowEvent::Resized(size) => self.state().resize(size),
            _ => (),
        }

        let state = self.state();
        state.renderer.handle_imgui_window_event(&state.window, &event);
    }

    fn device_event(&mut self, _event_loop: &ActiveEventLoop, _device_id: DeviceId, event: DeviceEvent) {
        let state = self.state.as_mut().unwrap();

        if let DeviceEvent::MouseMotion { delta } = event {
            state.handle_mouse(glam::vec2(delta.0 as f32, delta.1 as f32));
        }
    }
}