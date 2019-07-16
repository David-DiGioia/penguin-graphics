#version 460 core

layout(location = 0) in vec3 position;

void main()
{
	vec3 newPos = position;
    switch(gl_VertexID)
    {
    case 0:
        // Top-right
        break;
    case 1:
        // Bottom-right
        break;
    case 2:
        // Bottom-left
        break;
    }

	gl_Position = vec4(newPos, 1.0f);
}