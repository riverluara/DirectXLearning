#include "GameEntity.h"


GameEntity::GameEntity(Mesh* mesh_1, Material* mat1) {
	
	mesh = mesh_1;
	trans = {0.0f, 0.0f, 0.0f};
	scale = {1.0f, 1.0f, 1.0f};
	rot = {0.0f, 0.0f, 0.0f};
	worldMatrix =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f
	};
	material1 = mat1;
	

}
GameEntity::~GameEntity() {
	
}

void GameEntity::SetTranslation(float x, float y, float z) {
	trans = {x, y, z};
}
void GameEntity::SetScale(float x, float y, float z) {
	scale = { x, y, z };
}
void GameEntity::SetRotate(float x, float y, float z) {
	rot = { x, y, z };
}

void GameEntity::Move(float x, float y, float z) {
	SetTranslation(x, y, z);
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(x, y, z);
	DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(scaling * rotation * translation));
}
void GameEntity::Scale(float x, float y, float z) {
	SetScale(x, y, z);
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(trans.x, trans.y, trans.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
	DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(x, y, z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(scaling * rotation * translation));
}
void GameEntity::Rotate(float x, float y, float z) {
	SetRotate(x, y, z);
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(trans.x, trans.y, trans.z);
	DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(scaling * rotation * translation));
}

DirectX::XMFLOAT4X4* GameEntity::GetWorldMatrix() {
	
	return &worldMatrix;
}
ID3D11Buffer* GameEntity::GetMeshVertexBuffer() {
	return mesh->GetVertexBuffer();
	
}

ID3D11Buffer* GameEntity::GetMeshIndexBuffer() {
	return mesh->GetIndexBuffer();
}

void GameEntity::PrepareMaterial(std::string SamplerName, std::string SRVName, std::string NormalSRVName, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix) {
	DirectX::XMMATRIX w = DirectX::XMLoadFloat4x4(&worldMatrix);
	DirectX::XMVECTOR d = DirectX::XMMatrixDeterminant(w);
	DirectX::XMMATRIX world = DirectX::XMMatrixInverse(&d, w);
		
	world = DirectX::XMMatrixTranspose(world);
	DirectX::XMStoreFloat4x4(&transWorld, world);
	

	material1->VertexShaderSetMatrices(worldMatrix, viewMatrix, projectionMatrix, transWorld);
	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	material1->VertexShaderCopyAllBufferData();
	material1->SetSamplerState(SamplerName);
	material1->SetShaderResourceView(SRVName);
	material1->SetShaderResourceNormalMapView(NormalSRVName);

	material1->PixelShaderCopyAllBufferData(); // This sends data to GPU!!!
	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	material1->SetVertexShader();
	material1->SetPixelShader();
	
}

void GameEntity::Draw(ID3D11DeviceContext* context) {

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer1 = mesh->GetVertexBuffer();
	ID3D11Buffer* indexBuffer1 = mesh->GetIndexBuffer();
	context->IASetVertexBuffers(0, 1, &vertexBuffer1, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer1, DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(
		mesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}
