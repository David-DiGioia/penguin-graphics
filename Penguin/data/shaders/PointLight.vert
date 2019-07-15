#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_cameraSpacePos;

uniform mat4 u_modelToCamera;
//uniform mat4 u_cameraToClip;
uniform mat3 u_normalModelToCameraMatrix;

layout(std140, binding = 2) uniform Projection
{
	mat4 cameraToClipMatrix;
};


void main()
{
	gl_Position = cameraToClipMatrix * (u_modelToCamera * vec4(position, 1.0f));

	v_texCoord = texCoord;
	v_cameraSpacePos = vec3(u_modelToCamera * vec4(position, 1.0f));
	v_normal = u_normalModelToCameraMatrix * normal;
}