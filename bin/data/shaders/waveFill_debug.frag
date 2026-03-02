#version 150

uniform sampler2D maskTexture;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Sample dengan vTexCoord (normalized 0-1)
    vec4 maskSample = texture(maskTexture, vTexCoord);

    // Tampilkan mask sebagai grayscale
    float mask = maskSample.r;
    outputColor = vec4(mask, mask, mask, 1.0);
}
