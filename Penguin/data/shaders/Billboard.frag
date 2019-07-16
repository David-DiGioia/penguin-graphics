#version 460 core

in vec2 v_mapping;

layout(location = 0) out vec4 outputColor;

uniform float u_sphereRadius;
uniform vec3 u_cameraSpherePos;

layout(std140, binding = 0) uniform Material
{
	vec4 specularColor;
	float specularShininess;
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

vec4 computeLighting(in PerLight light, in vec4 diffuse, in vec3 cameraPos, in vec3 cameraNormal)
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
		float atten = calcAttenuation(cameraPos, light.cameraSpaceLightPos.xyz, lightDir);
		lightIntensity = atten * light.lightIntensity;
	}

	vec3 surfaceNormal = normalize(cameraNormal);
	float cosAngIncidence = dot(surfaceNormal, lightDir);
	cosAngIncidence = cosAngIncidence < 0.0001f ? 0.0 : cosAngIncidence;

	vec3 viewDirection = normalize(-cameraPos);

	vec3 halfAngle = normalize(lightDir + viewDirection);
	float angleNormalHalf = acos(dot(halfAngle, surfaceNormal));
	float exponent = angleNormalHalf / Mtl.specularShininess;
	exponent = -(exponent * exponent);
	float gaussianTerm = exp(exponent);

	gaussianTerm = cosAngIncidence != 0.0 ? gaussianTerm : 0.0f;

	vec4 lighting = diffuse * lightIntensity * cosAngIncidence;
	lighting += Mtl.specularColor * lightIntensity * gaussianTerm;

	return lighting;
}

void impostor(out vec3 cameraPos, out vec3 cameraNormal)
{
	vec3 cameraPlanePos = vec3(v_mapping * u_sphereRadius, 0.0) + u_cameraSpherePos;
	vec3 dir = normalize(cameraPlanePos);

	float B = 2.0f * dot(dir, -u_cameraSpherePos);
	float C = dot(u_cameraSpherePos, u_cameraSpherePos) - (u_sphereRadius * u_sphereRadius);

	float determinant = (B * B) - (4.0f * C);
	if (determinant < 0.0f)
		discard;

	float t = (-B - sqrt(determinant)) / 2.0f;

	cameraPos = dir * t;
	cameraNormal = normalize(cameraPos - u_cameraSpherePos);
}

void main()
{
	vec3 cameraPos;
	vec3 cameraNormal;
	
	vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	impostor(cameraPos, cameraNormal);
	
	vec4 accumLighting = diffuse * Lgt.ambientIntensity;
	for (int light = 0; light < numberOfLights; ++light)
	{
		accumLighting += computeLighting(Lgt.lights[light],
			diffuse, cameraPos, cameraNormal);
	}
	
	accumLighting /= Lgt.maxIntensity;
	outputColor = accumLighting;
	outputColor.a = 1.0f;
}