#version 150

uniform sampler2D tex;
uniform vec4 color;
varying vec2 texcoord;
varying float id;

void main()
{
	float alpha = texture2D(tex, texcoord).a;
	//gl_FragColor = vec4(1.0f);
	//gl_FragColor = vec4(id / 4.0f, 0.0f, 0.0f, 1.0f);
	gl_FragColor = vec4(1,1,1,1) * alpha;
	//gl_FragColor = vec4(texcoord, 0, 1);
	//gl_FragColor = vec4(1.0f, 1.0f, 1.0f, alpha) * color;
	//gl_FragColor = vec4(1.0f, 1.0f, 1.0f, alpha) * vec4(1);
}