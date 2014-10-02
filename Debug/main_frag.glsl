#version 150

uniform float time;
uniform sampler2D tex;
in vec2 fragTexCoord;
out vec4 color;

void main()
{
	color = texture(tex, fragTexCoord);
	color.r *= (sin(time * 4) + 1) / 2;
	color.g *= (cos(time * 4) + 1) / 2;
	color.b *= (sin(time * 4.5) + 1) / 2;
}