#include "Material.h"

Material::Material(SimpleVertexShader* vShader, SimplePixelShader* pShader) {
	vertexShader = vShader;
	pixelShader = pShader;
}
Material::~Material() {
	
	delete vertexShader;
	delete pixelShader;

}
void Material::VertexShaderSetMatrices(DirectX::XMFLOAT4X4 world, DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT4X4 transWorld) {

	vertexShader->SetMatrix4x4("world", world);
	vertexShader->SetMatrix4x4("view", view);
	vertexShader->SetMatrix4x4("projection", projection);
	vertexShader->SetMatrix4x4("transWorld", transWorld);
}

void Material::VertexShaderCopyAllBufferData() {

	vertexShader->CopyAllBufferData();
}

void Material::SetVertexShader() {
	
	vertexShader->SetShader();
}
void Material::SetPixelShader() {

	pixelShader->SetShader();
}