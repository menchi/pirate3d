// GLSL fragment shader

<GLSLBegin Name = "InvertColor">
vec4 PostProcessColor(vec4 inColor)
{
	return 1.0 - inColor;
}
</GLSLEnd>

<GLSLBegin Name = "HalfColor">
vec4 PostProcessColor(vec4 inColor)
{
	return inColor * 0.5;
}
</GLSLEnd>

<GLSLBegin Name = "PSMain">
vec4 PostProcessColor(vec4 inColor);

void main()
{
	gl_FragColor = PostProcessColor(gl_Color);
}
</GLSLEnd>