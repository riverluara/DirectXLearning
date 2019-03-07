
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float3 worldPos		: POSITION;
	float2 uv           : TEXCOORD;    //UV
	float3 tangent      : TANGENT;
};

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
struct DirectionaLight 
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};
cbuffer dLightData : register(b0)
{
	DirectionaLight dLight1;
	DirectionaLight dLight2;
	float3 PointLightPosition;
	float3 PointLightColor;

	float3 CameraPosition;
};

float4 GetCalculateColors(float3 normal, DirectionaLight light) {
	normal = normalize(normal);
	float negate = -1.0f;
	float3 direction = light.Direction * negate;
	direction = normalize(direction);
	float NdotL = dot(normal, direction);
	NdotL = saturate(NdotL);

	return light.AmbientColor + (light.DiffuseColor * NdotL);
}
Texture2D SRview : register(t0);// Texture
SamplerState sampState : register(s0);//How you use  your texture
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.normal);

	float3 normalMap = 
	float4 surfaceColor = SRview.Sample(sampState, input.uv);
	float shininess = 32.0f; // Arbitrary surface shininess value

	float3 dirToCamera = normalize(CameraPosition - input.worldPos);


	// POINT LIGHT //////////////////////////////////

	// Direction TO the point light from the surface
	float3 dirToPointLight = normalize(PointLightPosition - input.worldPos);

	float3 pointNdotL = dot(input.normal, dirToPointLight);
	pointNdotL = saturate(pointNdotL); // Remember to CLAMP between 0 and 1

	// Specular calc for reflections (Phong)
	float3 pointRefl = reflect(-dirToPointLight, input.normal);
	float pointSpec = pow(saturate(dot(pointRefl, dirToCamera)), shininess);

	// Combine the surface and lighting
	float3 finalPointLight =
		/*surfaceColor **/ PointLightColor * pointNdotL
		+ pointSpec.rrr; // Making the spec value into a float3


	


	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	
    float4 lightColor1 = GetCalculateColors(input.normal, dLight1);
	float4 lightColor2 = GetCalculateColors(input.normal, dLight2);
	return surfaceColor* (lightColor1 + lightColor2 + float4(finalPointLight, 1.0f));
}