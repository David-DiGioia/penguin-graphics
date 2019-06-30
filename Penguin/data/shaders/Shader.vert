#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

smooth out vec4 v_interpColor;
out vec2 v_texCoord;

uniform vec3 u_dirToLight;
uniform vec4 u_lightIntensity;

uniform mat4 u_modelToCamera;
uniform mat4 u_cameraToClip;
uniform mat3 u_normalModelToCameraMatrix;


void main()
{
	// TEMP
	vec4 diffuseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

	gl_Position = u_cameraToClip * u_modelToCamera * vec4(position, 1.0f);
	v_texCoord = texCoord;

	vec3 normCamSpace = normalize(u_normalModelToCameraMatrix * normal);

	float cosAngIncidence = dot(normCamSpace, u_dirToLight);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	v_interpColor = u_lightIntensity * diffuseColor * cosAngIncidence;
}