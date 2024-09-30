#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D screenTexture;  // Texture to be applied to the quad

void main() {
    FragColor = texture(screenTexture, TexCoord); // Sample the texture using the texture coordinates
}