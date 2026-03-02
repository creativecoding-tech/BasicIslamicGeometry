#version 460 core

uniform vec4 fillColor;
in vec2 vTexCoord;
out vec4 fragColor;

void main() {
    // Simple fill dengan warna polygon
    fragColor = fillColor;
}
