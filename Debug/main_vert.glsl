#version 150

in vec3 vert;
in vec2 vertTexCoord;
out vec2 fragTexCoord;
uniform float time;

void main()
{
	fragTexCoord = vertTexCoord;
	gl_Position = vec4(vert, 1);
	gl_Position.x += sin(time * 8 + gl_VertexID) * 0.2f;
	gl_Position.y += cos(time * 8 + gl_VertexID) * 0.2f;
	gl_Position.x *= sin(time * 4 + gl_VertexID);
	gl_Position.y *= cos(time * 4 + gl_VertexID);
}