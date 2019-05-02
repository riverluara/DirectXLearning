cbuffer externalData:register(b0) 
{
	matrix view;
	matrix projection;

};

struct VertexShaderInput
{
	float3 position		:POSITION;
	float2 uv			:TEXCOORD;
	float3 normal		:NORMAL;
	float3 tangent		:TANGENT;
};

struct VertexToPixel
{
	float4 position		:SV_POSITION;
	float3 cubeDirection	:DIRECTION;
};
VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;
	// Create a view matrix with NO translation
	matrix viewNoTranslation = view;
	viewNoTranslation._41 = 0;
	viewNoTranslation._42 = 0;
	viewNoTranslation._43 = 0;

	// Calculate output position
	matrix vp = mul(viewNoTranslation, projection);
	output.position = mul(float4(input.position, 1.0f), vp);

	// Push the vertices away from us, so that they
	// sit directly on the far clip plane (z = 1)
	output.position.z = output.position.w;

	// Set the direction to the vert
	output.cubeDirection = input.position;

	return output;

}