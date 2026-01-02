use std::time::Duration;
use imgui_winit_support::{HiDpiMode, WinitPlatform};
use winit::event::{Event, WindowEvent};
use winit::window::{Window, WindowId};
use crate::texture::DepthTexture;

pub struct ImGuiState {
    context: imgui::Context,
    renderer: imgui_wgpu::Renderer,
    platform: WinitPlatform,
}

impl ImGuiState {
    pub fn new(device: &wgpu::Device, queue: &wgpu::Queue, surface_format: wgpu::TextureFormat, window: &Window) -> Self {
        let mut context = imgui::Context::create();
        context.set_ini_filename(None);

        let renderer = imgui_wgpu::Renderer::new(&mut context, &device, &queue, imgui_wgpu::RendererConfig {
            texture_format: surface_format,
            depth_format: Some(DepthTexture::DEPTH_FORMAT),
            ..Default::default()
        });

        let mut platform = WinitPlatform::new(&mut context);
        platform.attach_window(context.io_mut(), window, HiDpiMode::Default);

        Self {
            context,
            renderer,
            platform,
        }
    }

    pub fn render<'a, F>(&'a mut self, window: &Window, device: &wgpu::Device, queue: &wgpu::Queue, render_pass: &mut wgpu::RenderPass<'a>, mut run_ui: F)
    where
        F: FnMut(&mut imgui::Ui)
    {
        self.platform.prepare_frame(self.context.io_mut(), window).unwrap();
        let ui = self.context.frame();
        run_ui(ui);
        self.platform.prepare_render(ui, window);
        let draw_data = self.context.render();

        self.renderer.render(draw_data, queue, device, render_pass).unwrap();
    }

    pub fn update_delta_time(&mut self, delta_time: f32) {
        self.context.io_mut().update_delta_time(Duration::from_secs_f32(delta_time));
    }

    pub fn handle_window_event(&mut self, window: &Window, window_id: WindowId, event: WindowEvent) {
        self.platform.handle_event::<()>(
            self.context.io_mut(),
            window,
            &Event::WindowEvent { window_id, event, }
        );
    }
}