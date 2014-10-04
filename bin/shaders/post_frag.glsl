#version 150

uniform sampler2D fbo_texture;
uniform sampler2D rbo_depth;
uniform float time;
varying vec2 f_texcoord;

const float sampleDist = 1.0;
const float sampleStrength = 2.2;

void main()
{
	vec2 uv = f_texcoord;
	//uv.x += sin(uv.y * 4 * 2 * 3.14159 + time) / 100;
	//uv.y += cos(uv.x * 4 * 2 * 3.14159 + time) / 100;

	float samples[10];
	samples[0] = -0.08;
	samples[1] = -0.05;
	samples[2] = -0.03;
	samples[3] = -0.02;
	samples[4] = -0.01;
	samples[5] = 0.01;
	samples[6] = 0.02;
	samples[7] = 0.03;
	samples[8] = 0.05;
	samples[9] = 0.08;

	vec2 dir = 0.5 - uv;
	float dist = sqrt(dir.x*dir.x + dir.y*dir.y);
	dir = dir / dist;

	vec4 color = texture2D(fbo_texture, uv);
	vec4 sum = color;

	for (int i = 0; i < 10; i++)
		sum += texture2D(fbo_texture, uv + dir * samples[i] * sampleDist);

	sum *= 1.0 / 11.0;
	float t = dist * sampleStrength;
	t = clamp(t, 0.0, 1.0);

	gl_FragColor = mix(color, sum, t);
	//vec2 texcoord = f_texcoord;
	//texcoord.x += sin(texcoord.y * 4 * 2 * 3.14159 + time) / 100;
	//texcoord.y += cos(texcoord.y * 4 * 2 * 3.14159 + time) / 100;
	//gl_FragColor = texture2D(fbo_texture, texcoord);
}