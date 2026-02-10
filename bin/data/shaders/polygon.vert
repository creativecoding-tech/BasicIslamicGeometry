#version 150

uniform mat4 modelViewProjectionMatrix;
uniform vec4 color;  // Uniform color instead of vertex attribute

in vec4 position;

out vec4 vColor;

void main() {
    vColor = color;
    gl_Position = modelViewProjectionMatrix * position;
}
