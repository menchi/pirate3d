// GLSL vertex shader

<GLSLBegin Name = "VSMain">
vec4 transform(vec4 inPosition);
void main()
{
	gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, 0.0, 1.0);
	gl_FrontColor = gl_Color;
}
</GLSLEnd>

<GLSLBegin Name = "Scale">
vec4 transform(vec4 inPosition)
{
	return vec4(inPosition.xyz * 0.5, inPosition.w);
}
</GLSLEnd>