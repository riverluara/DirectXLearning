#include "Material.h"

Material::Material(SimpleVertexShader* vShader, SimplePixelShader* pShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11ShaderResourceView* NormalMapSRView, ID3D11SamplerState* samplerState) {
	vertexShader = vShader;
	pixelShader = pShader;
	SRView = shaderResourceView;
	sampState = samplerState;
	NormalSRView = NormalMapSRView;
}
Material::~Material() {
	
	//if (vertexShader) {
	//	delete vertexShader;
	//}
	//if (pixelShader) {
	//	delete pixelShader;
	//}
	

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
void Material::PixelShaderCopyAllBufferData() {

	pixelShader->CopyAllBufferData();
}

void Material::SetVertexShader() {
	
	vertexShader->SetShader();
}
void Material::SetPixelShader() {

	pixelShader->SetShader();
}

ID3D11ShaderResourceView* Material::GetSRView() {

	return SRView;
}
ID3D11SamplerState* Material::GetSamplerState() {

	return sampState;
}
void Material::SetSamplerState(std::string name) {

	pixelShader->SetSamplerState(name, sampState);
}
void Material::SetShaderResourceView(std::string name) {
	pixelShader->SetShaderResourceView(name, SRView);
}
void Material::SetShaderResourceNormalMapView(std::string name) {
	pixelShader->SetShaderResourceView(name, NormalSRView);
}