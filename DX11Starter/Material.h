#pragma once
#include "SimpleShader.h"

class Material {
public:
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11ShaderResourceView* NormalMapSRView, ID3D11SamplerState* samplerState);
	~Material();

	void VertexShaderSetMatrices(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT4X4 transWorld);
	void VertexShaderCopyAllBufferData();
	void PixelShaderCopyAllBufferData();
	void SetVertexShader();
	void SetPixelShader();
	void SetSamplerState(std::string name);
	void SetShaderResourceView(std::string name);
	void SetShaderResourceNormalMapView(std::string name);
	ID3D11ShaderResourceView* GetSRView();
	ID3D11SamplerState* GetSamplerState();

private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	ID3D11ShaderResourceView* SRView;
	ID3D11ShaderResourceView* NormalSRView;
	ID3D11SamplerState* sampState;
};