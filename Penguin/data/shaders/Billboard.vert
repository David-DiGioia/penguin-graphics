#version 460

layout(std140) uniform;

out vec2 v_mapping;

layout(std140, binding = 2) uniform Projection
{
	mat4 cameraToClipMatrix;
};

uniform float u_sphereRadius;
uniform vec3 u_cameraSpherePos;

const float g_boxCorrection = 2.5f;

void main()
{
    vec2 offset;
    switch(gl_VertexID)
    {
    case 0:
        //Bottom-left
        v_mapping = vec2(-1.0, -1.0) * g_boxCorrection;
        offset = vec2(-u_sphereRadius, -u_sphereRadius);
        break;
    case 1:
        //Bottom-right
        v_mapping = vec2(1.0, -1.0) * g_boxCorrection;
        offset = vec2(u_sphereRadius, -u_sphereRadius);
        break;
    case 2:
        //Top-left
        v_mapping = vec2(-1.0, 1.0) * g_boxCorrection;
        offset = vec2(-u_sphereRadius, u_sphereRadius);
        break;
    case 3:
        //Top-right
        v_mapping = vec2(1.0, 1.0) * g_boxCorrection;
        offset = vec2(u_sphereRadius, u_sphereRadius);
        break;
    }

    vec4 cameraCornerPos = vec4(u_cameraSpherePos, 1.0);
    cameraCornerPos.xy += offset * g_boxCorrection;
    
    gl_Position = cameraToClipMatrix * cameraCornerPos;
}