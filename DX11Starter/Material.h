#pragma once
#include "SimpleShader.h"

class Material {
public:
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader);
	~Material();

	void VertexShaderSetMatrices(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection);
	void VertexShaderCopyAllBufferData();
	void SetVertexShader();
	void SetPixelShader();

private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
};