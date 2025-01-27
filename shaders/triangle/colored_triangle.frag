#version 450

// input from the vertex shader
layout (location = 0) in vec3 inColor;

// Output
layout (location = 0) out vec4 outFragColor;

void main() {
    //return color
    outFragColor = vec4(inColor, 1.f);
}