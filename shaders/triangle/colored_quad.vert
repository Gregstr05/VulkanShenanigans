#version 450

layout (location = 0) out vec3 outColor;

void main() {
    // constant array of the triangle vertices
    const vec3 positions[6] = vec3[6](
    vec3(1.f,1.f, 0.0f),
    vec3(-1.f,1.f, 0.0f),
    vec3(1.f,-1.f, 0.0f),
    vec3(1.f,-1.f, 0.0f),
    vec3(-1.f,1.f, 0.0f),
    vec3(-1.f,-1.f, 0.0f)
    );

    // constant array of colours (vertex colours)
    const vec3 colors[6] = vec3[6](
    vec3(1.0f, 0.0f, 0.0f), //red
    vec3(0.0f, 1.0f, 0.0f), //green
    vec3(00.f, 0.0f, 1.0f),  //blue
    vec3(00.f, 0.0f, 1.0f),  //blue
    vec3(00.f, 0.0f, 1.0f),  //blue
    vec3(00.f, 0.0f, 1.0f)  //blue
    );

    // Output the position and color of each vertex
    gl_Position = vec4(positions[gl_VertexIndex], 1.f);
    outColor = colors[gl_VertexIndex];
}