#version 150

uniform sampler2D maskTexture;
uniform vec4 fillColor;
uniform float time;
uniform float gradientSpeed;
uniform float gradientFrequency;
uniform vec2 uMaskSize;
uniform vec2 uBoundingBoxMin;
uniform vec2 uBoundingBoxMax;

in vec2 vTexCoord;
in float vWorldY;
out vec4 outputColor;

void main() {
    // Sample polygon mask
    vec4 maskSample = texture(maskTexture, vTexCoord);

    // Cek apakah di dalam polygon (mask RED channel > 0.5)
    if (maskSample.r < 0.5) {
        discard;
    }

    // Normalized position (0-1)
    vec2 normalizedPos = vTexCoord;

    // Gradient wave effect - variasi warna berdasarkan posisi dan waktu
    float gradient = sin(time * gradientSpeed + normalizedPos.x * gradientFrequency + normalizedPos.y * gradientFrequency);

    // Normalize gradient dari [-1,1] ke [0,1]
    float gradientNorm = (gradient + 1.0) * 0.5;  // Range 0.0 to 1.0

    // Buat variasi warna dari fillColor ke PUTIH
    vec4 baseColor = fillColor;                    // Warna asli
    vec4 whiteColor = vec4(1.0, 1.0, 1.0, fillColor.a);  // Putih

    // Mix antara warna asli dan putih berdasarkan gradient
    vec4 gradientColor = mix(baseColor, whiteColor, gradientNorm * 0.6);  // Max 60% ke putih

    // Output color dengan alpha asli
    outputColor = vec4(gradientColor.rgb, fillColor.a);
}
