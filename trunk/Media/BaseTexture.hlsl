//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position   : POSITION;   // vertex position 
	float2 TexCoord   : TEXCOORD0;  // tex coords
};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                      in float2 texCoord  : TEXCOORD0 )
{
	VS_OUTPUT Output;

	// transform position to clip space 
	Output.Position = mul(vPosition, mWorldViewProj);
	Output.TexCoord = texCoord;
	
	return Output;
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler2D tex0;
	
PS_OUTPUT pixelMain( float2 TexCoord : TEXCOORD0 ) 
{ 
	PS_OUTPUT Output;

	float4 col = tex2D( tex0, TexCoord );  // sample color map
	
	// multiply with diffuse and do other senseless operations
	Output.RGBColor = col;

	return Output;
}

