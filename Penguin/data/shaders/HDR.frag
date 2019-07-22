#version 460 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_cameraSpacePos;

layout(location = 0) out vec4 outputColor;

uniform sampler2D u_texture;
uniform sampler2D u_specMap;

layout(std140, binding = 0) uniform Material
{
	vec4 diffuseColor;
	vec4 specularColor;
	float specularShininess;
	int materialIndex;
	int useSpecMap;
} Mtl;

struct PerLight
{
	vec4 cameraSpaceLightPos;
	vec4 lightIntensity;
};

const int numberOfLights = 2;

layout(std140, binding = 1) uniform Light
{
	vec4 ambientIntensity;
	float lightAttenuation;
	float maxIntensity;
	float gamma;
	PerLight lights[numberOfLights];
} Lgt;




float calcAttenuation(in vec3 cameraSpacePos,
	in vec3 cameraSpaceLightPos,
	out vec3 lightDirection)
{
	vec3 lightDifference = cameraSpaceLightPos - cameraSpacePos;
	float lightDistanceSqr = dot(lightDifference, lightDifference);
	lightDirection = lightDifference * inversesqrt(lightDistanceSqr);

	return (1 / (1.0f + Lgt.lightAttenuation * lightDistanceSqr));
}

vec4 computeLighting(in PerLight light, in vec4 texColor, in float shininess)
{
	vec3 lightDir;
	vec4 lightIntensity;
	if (light.cameraSpaceLightPos.w == 0.0f)
	{
		lightDir = vec3(light.cameraSpaceLightPos);
		lightIntensity = light.lightIntensity;
	}
	else
	{
		float atten = calcAttenuation(v_cameraSpacePos, light.cameraSpaceLightPos.xyz, lightDir);
		lightIntensity = atten * light.lightIntensity;
	}

	vec3 surfaceNormal = normalize(v_normal);
	float cosAngIncidence = dot(surfaceNormal, lightDir);
	cosAngIncidence = cosAngIncidence < 0.0001f ? 0.0 : cosAngIncidence;

	vec3 viewDirection = normalize(-v_cameraSpacePos);

	vec3 halfAngle = normalize(lightDir + viewDirection);
	float angleNormalHalf = acos(dot(halfAngle, surfaceNormal));
	float exponent = angleNormalHalf / shininess;
	exponent = -(exponent * exponent);
	float gaussianTerm = exp(exponent);

	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0f;

	vec4 lighting = texColor * lightIntensity * cosAngIncidence;
	lighting += Mtl.specularColor * lightIntensity * gaussianTerm;

	return lighting;
}

void main()
{
	vec4 texColor = texture(u_texture, v_texCoord);
	float shininess = (Mtl.useSpecMap == 1) ? texture(u_specMap, v_texCoord).r : Mtl.specularShininess;

	vec4 accumLighting = texColor * Lgt.ambientIntensity;
	for (int light = 0; light < numberOfLights; ++light)
	{
		accumLighting += computeLighting(Lgt.lights[light], texColor, shininess);
	}

	accumLighting /= Lgt.maxIntensity;
	//outputColor = pow(accumLighting, vec4(Lgt.gamma, Lgt.gamma, Lgt.gamma, 1.0f));
	outputColor = accumLighting;

	outputColor.a = 1.0f;
}