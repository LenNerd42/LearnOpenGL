﻿#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Matrix multiplication is done from right to left.
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
