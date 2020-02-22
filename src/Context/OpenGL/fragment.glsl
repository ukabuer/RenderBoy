#version 330 core
in vec2 v_uv;
uniform int use_texture;
uniform sampler2D u_texture;
uniform vec4 base_color;
out vec4 color;

void main() {
    if (use_texture == 1) {
        color = texture(u_texture, v_uv);
    } else {
        color = base_color;
    }
}