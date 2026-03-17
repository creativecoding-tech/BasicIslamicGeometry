#version 330

uniform mat4 modelViewProjectionMatrix;
uniform float lineWidth;

in vec4 position;
in vec4 color;
in vec2 texcoord;

out VS_OUT {
    vec4 color;
    float lineWidth;
    vec2 texCoord;
} vs_out;

void main() {
    vs_out.color = color;
    vs_out.lineWidth = lineWidth;
    vs_out.texCoord = texcoord;
    gl_Position = position;
}
