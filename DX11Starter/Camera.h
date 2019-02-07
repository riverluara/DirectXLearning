#pragma once
#include <DirectXMath.h>
#include <Windows.h>
#include "DXCore.h"
using namespace DirectX;
class Camera{
public:
	Camera(unsigned int width, unsigned int height);
	~Camera();

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void Update(float deltaTiime);
	void SetCameraRotation(float rotationX, float rotationY);
	void UpdateProjectionMatrix(unsigned int width, unsigned int height);

	
private:

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMVECTOR cameraPosition;
	XMVECTOR cameraDirection;
	

	float cameraRotationX;
	float cameraRotationY;
};