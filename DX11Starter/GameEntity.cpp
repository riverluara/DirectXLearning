#include "GameEntity.h"

GameEntity::GameEntity(Mesh* mesh_1) {
	
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
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(x, y, z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(translation));
}
void GameEntity::Scale(float x, float y, float z) {
	DirectX::XMMATRIX scaling = DirectX::XMMatrixScaling(x, y, z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(scaling));
}
void GameEntity::Rotate(float x, float y, float z) {
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(x, y, z);
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixTranspose(rotation));
}

DirectX::XMFLOAT4X4 GameEntity::GetWorldMatrix() {
	
	return worldMatrix;
}
ID3D11Buffer* GameEntity::GetMeshVertexBuffer() {
	return mesh->GetVertexBuffer();
	
}

ID3D11Buffer* GameEntity::GetMeshIndexBuffer() {
	return mesh->GetIndexBuffer();
}