﻿#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Matrix multiplication is done from right to left.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    fragPos = vec3(view * model * vec4(aPos, 1.0));
    // Model matrix specifically for normal vectors.
    normal = mat3(transpose(inverse(view * model))) * aNormal;
    texCoords = aTexCoords;
}
