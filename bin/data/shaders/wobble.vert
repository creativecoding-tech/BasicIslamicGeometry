#version 460 core

uniform mat4 modelViewProjectionMatrix;
uniform vec2 uWobbleOffset;  // Wobble offset dari WobbleAnimation
uniform float uWobbleAmount;   // Besar wobble (amplitude)

in vec4 position;
out vec2 vTexCoord;

void main() {
    // Apply wobble offset ke vertex position
    vec4 wobblePos = position;
    wobblePos.xy += uWobbleOffset * uWobbleAmount;

    vTexCoord = position.xy;
    gl_Position = modelViewProjectionMatrix * wobblePos;
}
