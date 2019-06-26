#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_proj;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_proj * position;
	v_TexCoord = texCoord;
}