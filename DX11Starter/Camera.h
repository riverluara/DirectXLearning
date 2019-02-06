#pragma once
#include <DirectXMath.h>

class Camera{
public:
	Camera();
	Camera();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	void Update();
	
private:

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 cameraPosition;
	DirectX::XMVECTOR cameraDirection;

	float cameraRotationX;
	float cameraRotationY;
};