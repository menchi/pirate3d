// GLSL fragment shader

vec4 InvertColor(vec4 inColor);

void main()
{
	gl_FragColor = InvertColor(gl_Color);
}