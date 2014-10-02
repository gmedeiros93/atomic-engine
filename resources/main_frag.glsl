#version 150

uniform mat4 model;
uniform sampler2D tex;
uniform vec3 sun;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragVert;

out vec4 color;

void main()
{
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 fragPosition = vec3(model * vec4(fragVert, 1));
	float sunCoef = max(0.0f, dot(fragNormal, sun));

	color = vec4(sunCoef);
	//color = vec4(normal.xyz, 1);
    //color = sunCoef * vec4(1) * texture(tex, fragTexCoord);
}