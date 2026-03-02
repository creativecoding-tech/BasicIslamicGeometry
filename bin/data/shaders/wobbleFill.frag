#version 150

uniform sampler2D maskTexture;
uniform vec4 fillColor;
uniform float fillLevel;
uniform float minY;
uniform float maxY;
uniform vec2 uMaskSize;
uniform vec2 uWobbleOffset;

in vec2 vTexCoord;
in float vWorldY;
out vec4 outputColor;

void main() {
	// Sample mask dengan wobble offset
	vec2 pixelCoord = vTexCoord * uMaskSize;
	pixelCoord += uWobbleOffset;
	vec2 wobbleTexCoord = pixelCoord / uMaskSize;
	wobbleTexCoord = clamp(wobbleTexCoord, 0.0, 1.0);

	vec4 maskSample = texture(maskTexture, wobbleTexCoord);

	// Hanya render jika di dalam polygon (mask > 0.5)
	if (maskSample.r < 0.5) {
		discard;
	}

	// Screen space: Y=0 di ATAS, Y=max di BAWAH
	// normalizedY: 0.0 = atas, 1.0 = bawah
	// fillLevel 0.0 = kosong, 1.0 = penuh
	float normalizedY = (vWorldY - minY) / (maxY - minY);

	// Anti-aliased edge untuk smooth fill
	// Pixel di BAWAH (normalizedY besar) yang terisi
	// fillLevel 0.5 = 50% BAWAH terisi
	float edgeWidth = 0.02;
	float alpha = smoothstep(1.0 - fillLevel - edgeWidth, 1.0 - fillLevel + edgeWidth, normalizedY);

	// Set output color dengan gradual alpha
	outputColor = vec4(fillColor.rgb, fillColor.a * alpha);
}
