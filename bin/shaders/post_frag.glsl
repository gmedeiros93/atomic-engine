#version 150

uniform sampler2D colorTex;
uniform sampler2D depthTex;
varying vec2 texCoord;

//const float sampleDist = 1.0;
//const float sampleStrength = 2.2;

// replace with uniform
const float zNear = 0.1f;
const float zFar = 100.0f;

void main()
{
	vec4 color = texture(colorTex, texCoord);
	float depth = texture(depthTex, texCoord).x;

	// Convert from exponential depth to scalar depth
	depth = (2 * zNear) / (zFar + zNear - depth * (zFar - zNear));

	gl_FragColor = color;

	/*float samples[10];
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

	gl_FragColor = mix(color, sum, t);*/
}