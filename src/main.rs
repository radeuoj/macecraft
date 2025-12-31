use winit::event_loop::{ControlFlow, EventLoop};

fn main() {
    env_logger::init();

    let event_loop = EventLoop::new().unwrap();
    let mut app = macecraft::App::new();
    event_loop.run_app(&mut app).unwrap();
}
