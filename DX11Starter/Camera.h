#pragma once
#include <DirectXMath.h>
#include <Windows.h>
using namespace DirectX;
class Camera{
public:
	Camera();
	~Camera();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void Update();
	
private:

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMVECTOR cameraPosition;
	XMVECTOR cameraDirection;
	

	float cameraRotationX;
	float cameraRotationY;
};