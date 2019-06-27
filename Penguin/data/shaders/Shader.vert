#version 460 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 u_modelToCamera;
uniform mat4 u_cameraToClip;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_cameraToClip * u_modelToCamera * position;
	v_TexCoord = texCoord;
}