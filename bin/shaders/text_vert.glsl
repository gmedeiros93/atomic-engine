#version 120

uniform mat4 matrix;
attribute vec4 coord;
varying vec2 texcoord;
varying float id;

void main()
{
	gl_Position = matrix * vec4(coord.xy, 0.0f, 1.0f);
	//gl_Position = vec4(coord.xy, 0.0f, 1.0f);
	texcoord = coord.zw;
	id = gl_VertexID;
}