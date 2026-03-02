#version 150

uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec2 texcoord;

out vec2 vTexCoord;
out float vWorldY;  // World Y coordinate untuk water calculation

void main() {
    vTexCoord = texcoord;
    vWorldY = position.y;  // Pass world Y coordinate
    gl_Position = modelViewProjectionMatrix * position;
}
