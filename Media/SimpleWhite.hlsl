// part of the Irrlicht Engine Shader example.
// These simple Direct3D9 pixel and vertex shaders will be loaded by the shaders
// example. Please note that these example shaders don't do anything really useful. 
// They only demonstrate that shaders can be used in Irrlicht.

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
float4x4 mWorldViewProj;  // World * View * Projection transformation


// Vertex shader output structure
struct VS_OUTPUT
{
	float4 Position   : POSITION;   // vertex position 
};


VS_OUTPUT vertexMain( in float4 vPosition : POSITION )
{
	VS_OUTPUT Output;

	// transform position to clip space 
	Output.Position = mul(vPosition, mWorldViewProj);

	return Output;
}



// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


sampler2D tex0;
	
PS_OUTPUT pixelMain() 
{ 
	PS_OUTPUT Output;

	Output.RGBColor = 1.0;

	return Output;
}

