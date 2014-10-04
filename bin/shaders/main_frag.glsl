#version 150

#define MAX_LIGHTS 32

#define DIR_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

struct Surface
{
	vec3 diffuse;
	vec3 specular;

	vec3 normal;

	float shininess;
};

struct Material
{
	sampler2D texture;

	vec3 specularColor;
	vec3 diffuseColor;

	float shininess;
};

struct Light
{
	int type;

	// position or direction
	vec3 vec;

	// color
	vec3 ambient, diffuse, specular;

	// falloff
	float linear, quadratic;
};

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 color;

uniform mat4 model;
uniform vec3 cameraPos;
uniform Material material;

uniform int totalLights;
uniform Light lights[MAX_LIGHTS];

vec3 applyDirLight(Light light, Surface surf, vec3 viewDir);
vec3 applyPointLight(Light light, Surface surf, vec3 pos, vec3 viewDir);
vec3 applySpotLight(Light light, Surface surf, bool not);

void main()
{
	//color = vec4(totalLights == 0 ? 0.0f : 1.0f, 0, 0, 1);
	//return;
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = texture(material.texture, fragTexCoord);
	vec3 surfaceViewDir = normalize(cameraPos - surfacePos);
	
	Surface surf;

	surf.normal = normal;
	surf.diffuse = material.diffuseColor * surfaceColor.rgb;
	surf.specular = material.specularColor;
	surf.shininess = material.shininess;

	vec3 total = vec3(0.0f);

	for (int i = 0; i < totalLights; i++)
	{
		vec3 value;

		switch (lights[i].type)
		{
		case DIR_LIGHT:
			value = applyDirLight(lights[i], surf, surfaceViewDir);
			break;

		case POINT_LIGHT:
			value = applyPointLight(lights[i], surf, surfacePos, surfaceViewDir);
			break;

		case SPOT_LIGHT:
			value = applySpotLight(lights[i], surf, true);
			break;

		default:
			break;
		}

		total += value;
	}

	vec3 gamma = vec3(1.0f / 2.2f);
	color = vec4(pow(total, gamma), surfaceColor.a);
}

vec3 applyDirLight(Light light, Surface surf, vec3 viewDir)
{
	float diff = max(0.0f, dot(surf.normal, light.vec));
	float spec = 0.0f;

	if (diff > 0.0f)
	{
		vec3 reflectDir = reflect(-light.vec, surf.normal);
		spec = pow(max(0.0f, dot(viewDir, reflectDir)), surf.shininess);
	}

	vec3 ambient = light.ambient * surf.diffuse;
	vec3 diffuse = light.diffuse * diff * surf.diffuse;
	vec3 specular = light.specular * spec * surf.specular;

	return ambient + diffuse + specular;
}

vec3 applyPointLight(Light light, Surface surf, vec3 pos, vec3 viewDir)
{
	vec3 lightSub = light.vec - pos;
	vec3 lightDir = normalize(lightSub);

	float diff = max(0.0f, dot(surf.normal, lightDir));
	float spec = 0.0f;

	if (diff > 0.0f)
	{
		vec3 reflectDir = reflect(-lightDir, surf.normal);
		spec = pow(max(0.0f, dot(viewDir, reflectDir)), surf.shininess);
	}

	float distance = length(lightSub);

	float attenuation = 1.0f / (1.0f +
		light.linear * distance +
		light.quadratic * (distance * distance));

	vec3 diffuse = light.diffuse * diff * surf.diffuse;
	vec3 specular = light.specular * spec * surf.specular;

	return attenuation * (diffuse + specular);
}

vec3 applySpotLight(Light light, Surface surf, bool not)
{
	return vec3(1.0f, 0.0f, 1.0f);
}