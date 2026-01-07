struct VertexOutput {
    @builtin(position) clip_position: vec4<f32>,
    @location(0) tex_coords: vec2<f32>,
}

struct CameraUniform {
    view_proj: mat4x4<f32>,
}

@group(1) @binding(0)
var<uniform> camera: CameraUniform;

@vertex
fn vs_main(@location(0) in: u32) -> VertexOutput {
    let position = vec3<u32>(
        in & ((1 << 6) - 1),
        (in >> 6) & ((1 << 6) - 1),
        (in >> 12) & ((1 << 6) - 1),
    );

    let tex_coords = vec2<u32>(
        (in >> 18) & ((1 << 5) - 1),
        (in >> 23) & ((1 << 5) - 1),
    );

    var out: VertexOutput;
    out.tex_coords = vec2<f32>(f32(tex_coords.x) / 16.0, f32(tex_coords.y) / 16.0);
    out.clip_position = camera.view_proj * vec4<f32>(vec3<f32>(position), 1.0);
    return out;
}

@group(0) @binding(0)
var t_diffuse: texture_2d<f32>;

@group(0) @binding(1)
var s_diffuse: sampler;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return textureSample(t_diffuse, s_diffuse, in.tex_coords);
}