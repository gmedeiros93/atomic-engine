#version 150

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

in vec3 vert;
in vec2 vertTexCoord;
in vec3 vertNormal;

out vec2 fragTexCoord;
out vec3 fragNormal;
out vec3 fragVert;

void main()
{
    fragTexCoord = vertTexCoord;
	fragNormal = vertNormal;
	fragVert = vert;

    gl_Position = projection * view * model * vec4(vert, 1);
}