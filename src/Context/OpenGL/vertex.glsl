#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_uv;

uniform mat4 model_matrix;
uniform mat4 view_matrix;

out vec2 v_uv;

void main() {
    v_uv = a_uv;
    gl_Position = view_matrix * model_matrix * vec4(a_pos, 1.0);
}