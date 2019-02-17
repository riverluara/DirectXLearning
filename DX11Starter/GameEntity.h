#pragma once
#include "Mesh.h"
#include "Material.h"
class GameEntity {
public:
	GameEntity(Mesh* mesh_1, Material* mat1);
	~GameEntity();
	//
	void SetTranslation(float x, float y, float z);
	void SetScale(float x, float y, float z);
	void SetRotate(float x, float y, float z);

	//
	void Move(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotate(float x, float y, float z);
	
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	ID3D11Buffer* GetMeshVertexBuffer();
	ID3D11Buffer* GetMeshIndexBuffer();

	void PrepareMaterial(DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);
	void Draw(ID3D11DeviceContext* context);
private:
	Mesh* mesh;
	DirectX::XMFLOAT3 trans;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 transWorld;

	Material* material1;
};