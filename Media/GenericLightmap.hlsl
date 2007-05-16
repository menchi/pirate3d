//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position   : POSITION;   // vertex position 
	float2 TexCoordD  : TEXCOORD0;  // diffuse map tex coords
	float2 TexCoordL  : TEXCOORD1;  // lightmap tex coords
};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                      in float2 texCoordD : TEXCOORD0,
                      in float2 texCoordL : TEXCOORD1 )
{
	VS_OUTPUT Output;

	// transform position to clip space 
	Output.Position = mul(vPosition, mWorldViewProj);
	Output.TexCoordD = texCoordD;
	Output.TexCoordL = texCoordL;
	
	return Output;
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler tex0 : register(s0);
sampler tex1 : register(s1);


PS_OUTPUT pixelMain( float2 TexCoordD : TEXCOORD0,
					 float2 TexCoordL : TEXCOORD1 ) 
{ 
	PS_OUTPUT Output;

	float4 col = tex2D(tex0, TexCoordD) * tex2D(tex1, TexCoordL);
	
	// multiply with diffuse and do other senseless operations
	Output.RGBColor = col;

	return Output;
}