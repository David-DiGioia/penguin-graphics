#version 460 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_cameraSpacePos;

layout(location = 0) out vec4 outputColor;

uniform sampler2D u_texture;
uniform vec3 u_cameraSpaceLightPos;
uniform vec4 u_lightIntensity;
uniform vec4 u_ambientLightIntensity;

void main()
{
	vec3 distVec = u_cameraSpaceLightPos - v_cameraSpacePos;
	vec3 lightDir = normalize(distVec);

	float k = .20f;
	vec4 attenuatedIntensity = u_lightIntensity / (1.0f + k * dot(distVec, distVec));

	// We must normalize the normal since it's being interpolated
	float cosAngIncidence = dot(normalize(v_normal), lightDir);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec4 texColor = texture(u_texture, v_texCoord);

	outputColor = (texColor * attenuatedIntensity * cosAngIncidence) +
		(texColor * u_ambientLightIntensity);

	outputColor.a = 1.0f;
}