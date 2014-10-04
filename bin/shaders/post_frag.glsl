#version 150

uniform sampler2D fbo_texture;
uniform float time;
varying vec2 f_texcoord;

void main()
{
	vec2 texcoord = f_texcoord;
	texcoord.x += sin(texcoord.y * 4 * 2 * 3.14159 + time) / 100;
	gl_FragColor = texture2D(fbo_texture, texcoord);
}