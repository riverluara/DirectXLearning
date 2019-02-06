#include "Camera.h"

Camera::Camera() {
	cameraDirection = XMVectorSet(0, 0, 1, 0);
	cameraPosition = XMVectorSet(0, 0, -5, 0);
	cameraRotationX = 0.0f;
	cameraRotationY = 0.0f;
}

Camera::~Camera() {

}

void Camera::Update() {
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	if (GetAsyncKeyState('W') & 0x8000) {

		cameraPosition += cameraDirection * XMVectorReplicate(1.0f); // XMVectorReplicate(1.0f) Returns XMVector(1,1,1,1)
	}

	if (GetAsyncKeyState('S') & 0x8000) {

		cameraPosition -= cameraDirection * XMVectorReplicate(1.0f);
	}

	if (GetAsyncKeyState('A') & 0x8000) {

		
	}

	if (GetAsyncKeyState('D') & 0x8000) {

	}

	if (GetAsyncKeyState(VK_SPACE) & 0x8000) {

	}

	if (GetAsyncKeyState('X') & 0x8000) {

	}

	//X and Y Rotation
	XMVector3Rotate(cameraDirection, XMQuaternionRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0.0f));
	//Store to view matrix
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(XMMatrixLookToLH(cameraPosition, cameraDirection, up)));



}
