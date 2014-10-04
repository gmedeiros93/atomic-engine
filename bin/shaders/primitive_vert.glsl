#version 150

uniform mat4 projection;
uniform mat4 view;

in vec3 vert;
in vec4 vertColor;

out vec4 fragColor;

void main()
{
	fragColor = vertColor;
    gl_Position = projection * view * vec4(vert, 1);
}