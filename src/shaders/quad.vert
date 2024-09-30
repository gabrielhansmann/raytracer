#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos.xy, 0.0, 1.0); // Set the position of the vertex
    TexCoord = aTexCoord;                  // Pass the texture coordinates to the fragment shader
}