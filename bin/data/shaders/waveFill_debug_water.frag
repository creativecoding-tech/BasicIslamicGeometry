#version 150

uniform sampler2D maskTexture;
uniform vec4 fillColor;
uniform float minY;
uniform float maxY;
uniform float currentWaterY;
uniform vec2 uBoundingBoxMin;
uniform vec2 uBoundingBoxMax;

in vec2 vTexCoord;
out vec4 outputColor;

void main() {
    // Sample polygon mask
    vec4 maskSample = texture(maskTexture, vTexCoord);

    // Cek apakah di dalam polygon
    if (maskSample.r < 0.5) {
        discard;
    }

    // Debug: visualize water level
    float distFromWater = gl_FragCoord.y - currentWaterY;

    if (distFromWater >= 0.0) {
        // Di bawah water level - BIRU
        outputColor = vec4(0.0, 0.0, 1.0, 1.0);
    } else {
        // Di atas water level - KUNING
        outputColor = vec4(1.0, 1.0, 0.0, 1.0);
    }
}
