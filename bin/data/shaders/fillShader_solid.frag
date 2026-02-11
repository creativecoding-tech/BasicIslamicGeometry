#version 150

uniform vec4 fillColor;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Debug: Output solid color TANPA texture sampling
    outputColor = vec4(1.0, 0.0, 0.0, 1.0);  // RED solid
}
