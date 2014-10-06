#version 150

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform float time;
varying vec2 texCoord;

const float sampleDist = 1.0;
//const float sampleStrength = 2.2;
const float sampleStrength = 5.2;

// replace with uniform
const float zNear = 0.1f;
const float zFar = 100.0f;

float getDepth(vec2 coord)
{
	float depth = texture(depthTex, coord).x;
	return (2 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
	//vec4 color = texture(colorTex, texCoord);
	vec2 uv = texCoord;
	float depth = texture(depthTex, texCoord).x;
	// Convert from exponential depth to scalar depth
	depth = (2 * zNear) / (zFar + zNear - depth * (zFar - zNear));
	uv.x += sin(uv.y * time * 3.14159) * 0.001;
	uv.y += cos(uv.x * time * 3.14159) * 0.001;

	uv.x *= depth;
	uv.y *= depth;

	//vec4 color = texture(colorTex, texCoord);
	//gl_FragColor = color * (1.0f - getDepth(texCoord));
	//return;

	/*int samples = 16;
	vec3 sample_sphere[] = {
		vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
		vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
		vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
		vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
		vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
		vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
		vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
		vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
	};

	float occ = 0.0f;
	float depth = getDepth(texCoord);

	for (int i = 0; i < samples; i++)
	{
		float sample = getDepth(texCoord + sample_sphere[samples].xy);
		float diff = sample - depth;

		if (diff > 0.0f)
		{
			occ += diff;
		}
	}

	float dist = 0.1f;

	for (int i = 0; i < samples; i++)
	{
		float dx = sin(3.14159 * (i / samples)) * dist;
		float dy = cos(3.14159 * (i / samples)) * dist;

		float sample = getDepth(texCoord + vec2(dx, dy));
		float diff = sample - depth;

		if (diff > 0.0f)
			occ += diff;
	}

	occ /= 16.0f;
	//occ = clamp(occ, 0.0f, 1.0f);
	gl_FragColor = vec4(occ, 0.0f, 0.0f, 1.0f);
	//gl_FragColor = vec4(color.rgb * (1.0f - occ), color.a);
	//gl_FragColor = color;*/

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

	vec4 color = texture2D(colorTex, uv);
	vec4 sum = color;

	color.r /= sin(time);
	color.g /= cos(time);

	for (int i = 0; i < 10; i++)
		sum += texture2D(colorTex, uv + vec2(sin(time), cos(time))*0.1 + dir * samples[i] * sampleDist);

	sum *= 1.0 / 11.0;
	float t = dist * sampleStrength;
	t += sin(time * cos(time * 3.14159));
	t = clamp(t, 0.0, 1.0);

	gl_FragColor = mix(color, sum, t);
}