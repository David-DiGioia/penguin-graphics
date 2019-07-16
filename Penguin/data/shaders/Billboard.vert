#version 460

layout(std140) uniform;

out vec2 mapping;

layout(std140, binding = 2) uniform Projection
{
	mat4 cameraToClipMatrix;
};

uniform float u_sphereRadius;
uniform vec3 u_cameraSpherePos;

void main()
{
    vec2 offset;
    switch(gl_VertexID)
    {
    case 0:
        //Bottom-left
        mapping = vec2(-1.0, -1.0);
        offset = vec2(-u_sphereRadius, -u_sphereRadius);
        break;
    case 1:
        //Top-left
        mapping = vec2(-1.0, 1.0);
        offset = vec2(-u_sphereRadius, u_sphereRadius);
        break;
    case 2:
        //Bottom-right
        mapping = vec2(1.0, -1.0);
        offset = vec2(u_sphereRadius, -u_sphereRadius);
        break;
    case 3:
        //Top-right
        mapping = vec2(1.0, 1.0);
        offset = vec2(u_sphereRadius, u_sphereRadius);
        break;
    }
    
    //switch(gl_VertexID)
    //{
    //case 0:
	//	gl_Position = vec4(0.5f, 0.5f, 0.0f, 1.0f);
    //    break;
    //case 1:
	//	gl_Position = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    //    break;
    //case 2:
	//	gl_Position = vec4(0.5f, -0.5f, 0.0f, 1.0f);
    //    break;
    //}

    vec4 cameraCornerPos = vec4(u_cameraSpherePos, 1.0);
    cameraCornerPos.xy += offset;
    
    gl_Position = cameraToClipMatrix * cameraCornerPos;
}