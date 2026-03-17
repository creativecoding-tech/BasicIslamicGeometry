#version 330

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 modelViewProjectionMatrix;

in VS_OUT {
    vec4 color;
    float lineWidth;
} gs_in[];

out vec4 vColor;

void main() {
    // Ambil line endpoints
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

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

    // Line width (dalam pixels, convert ke NDC)
    float width = gs_in[0].lineWidth;

    // Get viewport aspect ratio
    float aspect = 1920.0 / 1080.0;  // Width / Height

    // Convert pixel width ke NDC dengan aspect ratio correction
    // Untuk horizontal lines (perp.x = 0), offset di Y → tidak perlu aspect correction
    // Untuk vertical lines (perp.y = 0), offset di X → perlu bagi aspect
    float pixelToNDC = (2.0 / 1080.0) * 0.5;
    vec2 aspectCorrection = vec2(1.0 / aspect, 1.0);
    vec2 offset = perp * width * pixelToNDC * aspectCorrection;

    // Generate quad di screen space, lalu project back

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
}
