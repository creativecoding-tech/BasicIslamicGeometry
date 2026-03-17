#version 330

uniform mat4 modelViewProjectionMatrix;
uniform float lineWidth;

in vec4 position;
in vec4 color;

out VS_OUT {
    vec4 color;
    float lineWidth;
} vs_out;

void main() {
    vs_out.color = color;
    vs_out.lineWidth = lineWidth;
    gl_Position = position;
}
