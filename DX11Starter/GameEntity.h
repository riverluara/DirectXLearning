#pragma once
#include "Mesh.h"

class GameEntity {
public:
	GameEntity(Mesh* mesh_1);
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

private:
	Mesh* mesh;
	DirectX::XMFLOAT3 trans;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rot;
	DirectX::XMFLOAT4X4 worldMatrix;


};