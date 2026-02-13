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

    // Pulsing gradient effect - breathing/brightness flow
    float pulse = sin(time * gradientSpeed + normalizedPos.x * gradientFrequency + normalizedPos.y * gradientFrequency);

    // Create smooth brightness variation
    float brightness = 0.5 + pulse * 0.5;  // Range 0.0 to 1.0

    // Apply ke base color
    vec4 finalColor = fillColor;
    finalColor.r = clamp(finalColor.r * brightness, 0.0, 1.0);
    finalColor.g = clamp(finalColor.g * brightness, 0.0, 1.0);
    finalColor.b = clamp(finalColor.b * brightness, 0.0, 1.0);
    finalColor.a = fillColor.a;

    outputColor = finalColor;
}
