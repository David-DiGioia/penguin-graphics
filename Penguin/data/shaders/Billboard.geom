#version 460

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

layout(std140, binding = 2) uniform Projection
{
	mat4 cameraToClipMatrix;
};

in VertexData
{
	vec3 cameraSpherePos;
	float sphereRadius;
	uint materialIndex;
} vert[];

out FragData
{
	flat vec3 cameraSpherePos;
	flat float sphereRadius;
	flat uint materialIndex;
	smooth vec2 mapping;
};

const float g_boxCorrection = 2.5f;

void main()
{
	vec4 cameraCornerPos;
	// Bottom-left
	mapping = vec2(-1.0f, -1.0f) * g_boxCorrection;
	cameraSpherePos = vec3(vert[0].cameraSpherePos);
	sphereRadius = vert[0].sphereRadius;
	cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0f);
	cameraCornerPos.xy += vec2(-vert[0].sphereRadius, -vert[0].sphereRadius) * g_boxCorrection;
	gl_Position = cameraToClipMatrix * cameraCornerPos;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	materialIndex = vert[0].materialIndex;
	EmitVertex();
	
	// Bottom-right
	mapping = vec2(1.0, -1.0) * g_boxCorrection;
	cameraSpherePos = vec3(vert[0].cameraSpherePos);
	sphereRadius = vert[0].sphereRadius;
	cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
	cameraCornerPos.xy += vec2(vert[0].sphereRadius, -vert[0].sphereRadius) * g_boxCorrection;
	gl_Position = cameraToClipMatrix * cameraCornerPos;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	materialIndex = vert[0].materialIndex;
	EmitVertex();

	// Top-left
	mapping = vec2(-1.0, 1.0) * g_boxCorrection;
	cameraSpherePos = vec3(vert[0].cameraSpherePos);
	sphereRadius = vert[0].sphereRadius;
	cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
	cameraCornerPos.xy += vec2(-vert[0].sphereRadius, vert[0].sphereRadius) * g_boxCorrection;
	gl_Position = cameraToClipMatrix * cameraCornerPos;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	materialIndex = vert[0].materialIndex;
	EmitVertex();
	
	// Top-right
	mapping = vec2(1.0, 1.0) * g_boxCorrection;
	cameraSpherePos = vec3(vert[0].cameraSpherePos);
	sphereRadius = vert[0].sphereRadius;
	cameraCornerPos = vec4(vert[0].cameraSpherePos, 1.0);
	cameraCornerPos.xy += vec2(vert[0].sphereRadius, vert[0].sphereRadius) * g_boxCorrection;
	gl_Position = cameraToClipMatrix * cameraCornerPos;
	gl_PrimitiveID = gl_PrimitiveIDIn;
	materialIndex = vert[0].materialIndex;
	EmitVertex();
}