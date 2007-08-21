// GLSL vertex shader

void main()
{
	gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, 0.0, 1.0);
	gl_FrontColor = gl_Color;
}