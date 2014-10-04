#version 150

uniform mat4 model;
uniform vec3 cameraPos;

uniform struct Material
{
	sampler2D texture;
	float shininess;
	vec3 specularColor;
	vec3 diffuseColor;
} material;

uniform struct Light
{
	vec3 position;
	vec3 color;
	float ambient;
	float attenuation;
} light;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 color;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = texture(material.texture, fragTexCoord);
	vec3 surfaceToLight = normalize(light.position - surfacePos);
	vec3 surfaceToCamera = normalize(cameraPos - surfacePos);
	
	// ambient
	vec3 ambient = surfaceColor.rgb * light.ambient * light.color;

	// diffuse
	float diffuseCoef = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoef * surfaceColor.rgb * material.diffuseColor * light.color;

	// specular
	float specularCoef = 0.0f;
	if (diffuseCoef > 0.0)
		specularCoef = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), material.shininess);
	vec3 specular = specularCoef * material.specularColor * light.color;

	// attenuation
	float distanceToLight = length(light.position - surfacePos);
	float attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));

	// linear color (before gamma correction)
	vec3 linear = ambient + attenuation * (diffuse + specular);

	// final color
	vec3 gamma = vec3(1.0 / 2.2);
	color = vec4(pow(linear, gamma), surfaceColor.a);
}