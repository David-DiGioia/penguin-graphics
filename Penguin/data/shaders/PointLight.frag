#version 460 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_modelSpacePosition;

layout(location = 0) out vec4 outputColor;

uniform sampler2D u_texture;
uniform vec3 u_modelSpaceLightPos;
uniform vec4 u_lightIntensity;
uniform vec4 u_ambientLightIntensity;

void main()
{
	vec3 lightDir = normalize(u_modelSpaceLightPos - v_modelSpacePosition);

	// We must normalize the normal since it's being interpolated
	float cosAngIncidence = dot(normalize(v_normal), lightDir);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec4 texColor = texture(u_texture, v_texCoord);

	outputColor = (texColor * u_lightIntensity * cosAngIncidence) +
		(texColor * u_ambientLightIntensity);

	outputColor.a = 1.0f;
}