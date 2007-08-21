// 2D Polygon and vertex color

void VSMain(in  float2 iPosition	:POSITION,
			in  float4 iColor		:COLOR,
			out float4 oPosition	:POSITION,
			out float4 oColor		:COLOR)
{
	oPosition = float4(iPosition, 0.5f, 1.0f);
	oColor = iColor;
}
vertexfragment TwoToThree = compile_fragment vs_3_0 VSMain();

void PSInvertColor(in  float4 iColor :COLOR,
				   out float4 oColor :COLOR)
{ 
	oColor = 1.0f - iColor;
}
pixelfragment InvertColor = compile_fragment ps_3_0 PSInvertColor();

void PSHalfColor(in  float4 iColor :COLOR,
				 out float4 oColor :COLOR)
{ 
	oColor = iColor * 0.5f;
}
pixelfragment HalfColor = compile_fragment ps_3_0 PSHalfColor();

void PSMain(in  float4 iColor :COLOR,
			out float4 oColor :COLOR)
{ 
	oColor = iColor;
}
pixelfragment PassColor = compile_fragment ps_3_0 PSMain();