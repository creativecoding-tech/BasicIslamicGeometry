#version 150

uniform sampler2DRect maskTexture;  // Polygon mask texture

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Sample mask texture menggunakan vTexCoord
    vec4 maskSample = texture(maskTexture, vTexCoord);

    // Debug: tampilkan mask sebagai grayscale
    float mask = maskSample.r;
    outputColor = vec4(mask, mask, mask, 1.0);
}
