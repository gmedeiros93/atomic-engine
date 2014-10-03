#version 150

uniform mat4 model;
uniform vec3 cameraPos;
uniform vec3 lightPos;

uniform sampler2D tex;
uniform vec3 sun;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 color;

void main()
{
	vec3 lightColor = vec3(1);
	float lightAmbient = 0.05f;

	vec3 matSpecularColor = vec3(1);
	float matShininess = 80.0f;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 surfacePos = vec3(model * vec4(fragVert, 1));
	vec4 surfaceColor = texture(tex, fragTexCoord);
	vec3 surfaceToLight = normalize(lightPos - surfacePos);
	vec3 surfaceToCamera = normalize(cameraPos - surfacePos);

	// ambient
	vec3 ambient = lightAmbient * surfaceColor.rgb * lightColor;

	// diffuse
	float diffuseCoef = max(0.0, dot(normal, surfaceToLight));
	vec3 diffuse = diffuseCoef * surfaceColor.rgb * lightColor;

	// specular
	float specularCoef = 0.0f;
	if (diffuseCoef > 0.0)
		specularCoef = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), matShininess);
	vec3 specular = specularCoef * matSpecularColor * lightColor;

	// attenuation
	float attenuation = 1.0f;

	// linear color (before gamma correction)
	vec3 linear = ambient + attenuation * (diffuse + specular);

	// final color
	vec3 gamma = vec3(1.0 / 2.2);
	color = vec4(pow(linear, gamma), surfaceColor.a);
}