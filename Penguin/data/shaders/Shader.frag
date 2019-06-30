#version 460 core

layout(location = 0) out vec4 outputColor;

in vec4 v_interpColor;
in vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);

	outputColor = texColor * v_interpColor;
	outputColor.a = 1.0f;
}