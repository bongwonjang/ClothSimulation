#version 430

in vec3 FragPos;
in vec3 outNormal;
in vec2 TexCoord; //uv
in vec4 shadowTexCoord;
in vec4 FragPosLightSpace;

out vec4 pixel_color;

uniform vec3 viewPos;
uniform sampler2D depTex;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDir)
{
	float bias = max(0.005f * (1.0 - dot(outNormal, lightDir)), 0.0005f);
	float visibility = texture(depTex, shadowTexCoord.xy).z < shadowTexCoord.z - bias ? 1.0f : 0.1f;

	float closestDepth = texture(depTex, shadowTexCoord.xy).r;
	float currentDepth = shadowTexCoord.z;

	vec2 texelSize = 1.0 / textureSize(depTex, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(depTex, shadowTexCoord.xy + vec2(x, y) * texelSize).r;
			visibility += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	visibility /= 9.0;

	return visibility;
}

void main()
{
	vec3 lightPos = vec3(0.0f, 20.0f, 10.0f);
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

	//ambient
	float ambientStrength = 0.5f;
	vec3 ambient = ambientStrength * lightColor;

	//diffuse
	vec3 norm = -normalize(outNormal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular
	float specularStrength = 5.0f;
	vec3 viewDir = normalize(vec3(0.0f, 1.0f, 30.0f) - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
	vec3 specular = specularStrength * spec * lightColor;
	
	//shadow
	float shadow = ShadowCalculation(FragPosLightSpace, lightDir);
	
	vec3 color = vec3(0.16f, 0.02f, 0.02f);
	color = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;

	pixel_color = vec4(color, 1.0f);
}