#version 150

attribute vec2 vertex;
varying vec2 texCoord;

void main()
{
	gl_Position = vec4(vertex, 0.0, 1.0);
	texCoord = (vertex + 1.0) / 2.0;
}