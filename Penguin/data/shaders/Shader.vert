#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

//smooth out vec4 interpColor;

//uniform vec3 dirToLight;
//uniform vec4 lightIntensity;

uniform mat4 u_modelToCamera;
uniform mat4 u_cameraToClip;

out vec2 v_TexCoord;

void main()
{
	gl_Position = u_cameraToClip * u_modelToCamera * vec4(position, 1.0f);
	v_TexCoord = texCoord;

	//vec3 normCamSpace = normalize(normalModelToCameraMatrix * normal);

	//float cosAngIncidence = dot(normCamSpace, dirToLight);
	//cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	//interpColor = lightIntensity * diffuseColor * cosAngIncidence
}