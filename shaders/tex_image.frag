#version 450

// shader inputs
layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inUV;

// shader outputs
layout (location = 0) out vec4 outColor;

// Texture to access
layout (set = 0, binding = 0) uniform sampler2D displayTexture;

void main() {
    outColor = texture(displayTexture, inUV);
}