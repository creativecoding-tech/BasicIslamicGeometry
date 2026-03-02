#version 150

uniform sampler2DRect maskTexture;  // Polygon mask texture
uniform vec4 fillColor;             // Fill color with alpha
uniform float minY;                 // Top Y of polygon
uniform float maxY;                 // Bottom Y of polygon
uniform float currentWaterY;        // Current water level Y
uniform float waveAmplitude;        // Wave amplitude
uniform float waveFrequency;        // Wave frequency
uniform float progress;             // Animation progress (0-1)
uniform float aaMargin;             // Anti-aliasing margin
uniform vec2 resolution;            // Screen resolution

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Sample polygon mask - gunakan texture untuk sampler2DRect
    vec4 maskSample = texture(maskTexture, gl_FragCoord.xy);

    // Cek apakah pixel di dalam polygon (red channel > 0.5)
    float insidePolygon = maskSample.r;

    // Jika outside polygon, discard
    if (insidePolygon < 0.5) {
        discard;
    }

    // Calculate wave offset at this X position
    float waveOffset = sin((gl_FragCoord.x * waveFrequency * 0.01) + progress * waveFrequency * 6.28318);
    waveOffset *= waveAmplitude;

    // Apply wave amplitude decay near end
    float decay = 1.0;
    if (progress > 0.9) {
        float over90 = (progress - 0.9) / 0.1;
        decay = 1.0 - over90;
    }
    waveOffset *= decay;

    // Calculate effective water level with wave
    float effectiveWaterY = currentWaterY + waveOffset;

    // Calculate distance from pixel to wave surface
    float distFromWater = gl_FragCoord.y - effectiveWaterY;

    // Check if below wave
    if (distFromWater >= 0.0) {
        // Below wave - render filled
        if (distFromWater >= aaMargin) {
            // Far from boundary - solid color
            outputColor = fillColor;
        } else {
            // Near boundary - anti-aliasing
            float aaRatio = clamp(distFromWater / aaMargin, 0.0, 1.0);
            outputColor = vec4(fillColor.rgb, fillColor.a * aaRatio);
        }
    } else {
        // Above wave - transparent
        outputColor = vec4(0.0, 0.0, 0.0, 0.0);
    }
}
