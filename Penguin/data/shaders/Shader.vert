#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_modelToWorld;
uniform mat4 u_worldToClip;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_worldToClip * u_modelToWorld * position;
	v_TexCoord = texCoord;
}