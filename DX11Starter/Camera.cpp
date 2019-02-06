#include "Camera.h"

Camera::Camera() {

}

Camera::Camera() {

}

void Camera::Update() {
	//X and Y Rotation
	DirectX::XMVector3Rotate(cameraDirection, DirectX::XMQuaternionRotationRollPitchYaw(cameraRotationX, cameraRotationY, 0.0f));

}
