#include "Camera.h"

Camera::Camera(unsigned int width, unsigned int height) {
	cameraDirection = XMVectorSet(0, 0, 1, 0);
	cameraPosition = XMVectorSet(0, 0, -5, 0);
	cameraRotationX = 0.0f;
	cameraRotationY = 0.0f;
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

Camera::~Camera() {


}

void Camera::Update(float deltaTime) {
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMVECTOR down = XMVectorSet(0, -1, 0, 0);
	
	if (GetAsyncKeyState('W') & 0x8000) {

		cameraPosition += cameraDirection * deltaTime; // XMVectorReplicate(1.0f) Returns XMVector(1,1,1,1)
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		cameraPosition -= cameraDirection * deltaTime;
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		cameraPosition += XMVector3Cross(cameraDirection, up) * deltaTime;
	}

	if (GetAsyncKeyState('D') & 0x8000) {

		cameraPosition += XMVector3Cross(cameraDirection, down) * deltaTime;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {

		cameraPosition += up * deltaTime;
	}

	if (GetAsyncKeyState('X') & 0x8000) {

		cameraPosition += down * deltaTime;
	}

	//X and Y Rotation
	cameraDirection = XMVector3Rotate(cameraDirection, XMQuaternionRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0.0f));
	//Store to view matrix
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookToLH(cameraPosition, cameraDirection, up)));

	cameraRotationX = 0.0f;
	cameraRotationY = 0.0f;

}
void Camera::SetCameraRotation(float rotationX, float rotationY) {

	cameraRotationY += rotationX;
	cameraRotationX += rotationY;

}
XMFLOAT4X4 Camera::GetViewMatrix() {
	
	return viewMatrix;
}
XMFLOAT4X4 Camera::GetProjectionMatrix() {
	
	return projectionMatrix;
}

XMFLOAT3 Camera::GetCameraPosition() {

	XMFLOAT3 currentPosition;
	XMFLOAT4 position;
	XMStoreFloat4(&position, cameraPosition);
	currentPosition.x = position.x;
	currentPosition.y = position.y;
	currentPosition.z = position.z;
	return currentPosition;
}
void Camera::UpdateProjectionMatrix(unsigned int width, unsigned int height) {
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}
