#version 460

layout(location = 0) in vec3 worldSpherePos;
layout(location = 1) in float sphereRadius;
layout(location = 2) in uint materialIndex;

uniform mat4 u_worldToCamera;

out VertexData
{
	vec3 cameraSpherePos;
	float sphereRadius;
	uint materialIndex;
} outData;

void main()
{
	outData.cameraSpherePos = (u_worldToCamera * vec4(worldSpherePos, 1.0f)).xyz;
	outData.sphereRadius = sphereRadius;
	outData.materialIndex = materialIndex;
}