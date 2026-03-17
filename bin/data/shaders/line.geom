#version 330

#define TWO_PI 6.28318530718

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 modelViewProjectionMatrix;
uniform float progress;       // 0.0 - 1.0 untuk grow animation
uniform float waveAmplitude;  // Wave amplitude (0 = disable)
uniform float waveFrequency;  // Wave frequency
uniform float waveTime;       // Wave time accumulator (0.0 - 1.0 loop)
uniform float aspectRatio;    // Viewport width / height ratio

in VS_OUT {
    vec4 color;
    float lineWidth;
    vec2 texCoord;
} gs_in[];

out vec4 vColor;

void main() {
    // Ambil line endpoints dengan texCoord
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    float t0 = gs_in[0].texCoord.x;
    float t1 = gs_in[1].texCoord.x;

    // ⭐ WAVE ANIMATION: Apply wave displacement jika amplitude > 0
    // Formula SAMA dengan WaveLineAnimation.cpp: sin(frequency * TWO_PI * (t - progress))
    if (waveAmplitude > 0.001) {
        // Calculate wave direction (perpendicular ke line)
        vec2 lineDir = normalize(p1.xy - p0.xy);
        vec2 perp = vec2(-lineDir.y, lineDir.x);

        // Calculate wave phase menggunakan formula yang sama dengan CPU
        // WaveLineAnimation: phase = frequency * TWO_PI * (t - progress)
        float wavePhase0 = waveFrequency * TWO_PI * (t0 - waveTime);
        float wavePhase1 = waveFrequency * TWO_PI * (t1 - waveTime);

        // Calculate wave magnitude (-1.0 sampai 1.0)
        float waveMagnitude0 = sin(wavePhase0);
        float waveMagnitude1 = sin(wavePhase1);

        // Apply wave displacement ke arah perpendicular
        p0.xy += perp * waveMagnitude0 * waveAmplitude;
        p1.xy += perp * waveMagnitude1 * waveAmplitude;
    }

    // Convert ke screen space untuk correct thickness
    vec4 sp0 = modelViewProjectionMatrix * p0;
    vec4 sp1 = modelViewProjectionMatrix * p1;

    // Perspective divide untuk NDC
    vec2 ndc0 = sp0.xy / sp0.w;
    vec2 ndc1 = sp1.xy / sp1.w;

    // Direction vector di NDC
    vec2 dir = ndc1 - ndc0;
    float len = length(dir);

    if (len < 0.0001) {
        // Line terlalu pendek, skip
        return;
    }

    dir = normalize(dir);

    // Perpendicular vector
    vec2 perp = vec2(-dir.y, dir.x);

    // Line width (dalam pixels)
    float width = gs_in[0].lineWidth;

    // Convert pixel width ke NDC
    // NDC range is [-1, 1], so 2.0 units total
    // For consistent thickness in both orientations, correct for aspect ratio
    float pixelToNDC = 2.0 / 1080.0;

    // ⭐ Scale factor untuk mengatur ketebalan overall (0.5 = setengah dari ketebalan asli)
    float thicknessScale = 0.5;
    pixelToNDC *= thicknessScale;

    vec2 offset = perp * width * pixelToNDC;

    // ⭐ ASPECT RATIO CORRECTION untuk thickness konsisten di portrait/landscape
    // Di NDC, X dan Y punya range sama tapi pixel count beda
    // Kita bagi offset.x dengan aspectRatio untuk kompensasi
    offset.x /= aspectRatio;

    // ⭐ PROGRESS CLIPPING untuk grow animation
    // Jika seluruh line di luar progress, skip
    if (t0 > progress && t1 > progress) {
        return;  // Line belum sampai sini
    }

    // Jika seluruh line di dalam progress, gambar full
    if (t0 <= progress && t1 <= progress) {
        // Vertex 0: p0 - offset
        vec2 v0_ndc = ndc0 - offset;
        gl_Position = vec4(v0_ndc * sp0.w, sp0.z, sp0.w);
        vColor = gs_in[0].color;
        EmitVertex();

        // Vertex 1: p0 + offset
        vec2 v1_ndc = ndc0 + offset;
        gl_Position = vec4(v1_ndc * sp0.w, sp0.z, sp0.w);
        vColor = gs_in[0].color;
        EmitVertex();

        // Vertex 2: p1 - offset
        vec2 v2_ndc = ndc1 - offset;
        gl_Position = vec4(v2_ndc * sp1.w, sp1.z, sp1.w);
        vColor = gs_in[1].color;
        EmitVertex();

        // Vertex 3: p1 + offset
        vec2 v3_ndc = ndc1 + offset;
        gl_Position = vec4(v3_ndc * sp1.w, sp1.z, sp1.w);
        vColor = gs_in[1].color;
        EmitVertex();

        EndPrimitive();
    } else {
        // ⭐ Partial line: progress di antara t0 dan t1
        // Interpolate posisi di progress
        float alpha = (progress - t0) / (t1 - t0);
        vec4 p_mid = mix(p0, p1, alpha);
        vec4 sp_mid = modelViewProjectionMatrix * p_mid;
        vec2 ndc_mid = sp_mid.xy / sp_mid.w;

        // Tentukan endpoint mana yang progress-nya lebih kecil
        if (t0 <= progress) {
            // p0 visible, potong di p_mid
            vec2 v0_ndc = ndc0 - offset;
            gl_Position = vec4(v0_ndc * sp0.w, sp0.z, sp0.w);
            vColor = gs_in[0].color;
            EmitVertex();

            vec2 v1_ndc = ndc0 + offset;
            gl_Position = vec4(v1_ndc * sp0.w, sp0.z, sp0.w);
            vColor = gs_in[0].color;
            EmitVertex();

            vec2 v2_ndc = ndc_mid - offset;
            gl_Position = vec4(v2_ndc * sp_mid.w, sp_mid.z, sp_mid.w);
            vColor = gs_in[0].color;
            EmitVertex();

            vec2 v3_ndc = ndc_mid + offset;
            gl_Position = vec4(v3_ndc * sp_mid.w, sp_mid.z, sp_mid.w);
            vColor = gs_in[0].color;
            EmitVertex();

            EndPrimitive();
        } else {
            // p_mid visible, potong di p1
            vec2 v0_ndc = ndc_mid - offset;
            gl_Position = vec4(v0_ndc * sp_mid.w, sp_mid.z, sp_mid.w);
            vColor = gs_in[1].color;
            EmitVertex();

            vec2 v1_ndc = ndc_mid + offset;
            gl_Position = vec4(v1_ndc * sp_mid.w, sp_mid.z, sp_mid.w);
            vColor = gs_in[1].color;
            EmitVertex();

            vec2 v2_ndc = ndc1 - offset;
            gl_Position = vec4(v2_ndc * sp1.w, sp1.z, sp1.w);
            vColor = gs_in[1].color;
            EmitVertex();

            vec2 v3_ndc = ndc1 + offset;
            gl_Position = vec4(v3_ndc * sp1.w, sp1.z, sp1.w);
            vColor = gs_in[1].color;
            EmitVertex();

            EndPrimitive();
        }
    }
}
