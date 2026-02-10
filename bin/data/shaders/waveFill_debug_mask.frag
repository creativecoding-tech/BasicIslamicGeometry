#version 150

uniform sampler2D maskTexture;
uniform vec4 fillColor;
uniform vec2 uBoundingBoxMin;
uniform vec2 uBoundingBoxMax;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Sample polygon mask
    vec4 maskSample = texture(maskTexture, vTexCoord);

    // Debug: tampilkan mask sebagai warna MERAH jika ada, HITAM jika tidak ada
    if (maskSample.r > 0.5) {
        // Di dalam polygon - MERAH
        outputColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        // Di luar polygon - HITAM
        outputColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
