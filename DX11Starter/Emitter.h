#pragma once
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#include "Camera.h"
#include "SimpleShader.h"

struct Particle
{
	// 4 floats:
	float SpawnTime;
	DirectX::XMFLOAT3 StartPosition;

	// 4 floats:
	DirectX::XMFLOAT3 StartVelocity;
	float RotationStart;

	// 4 floats:
	float RotationEnd;
	DirectX::XMFLOAT3 padding;
};


class Emitter
{
public:
	Emitter(
		int maxParticles,
		int particlesPerSecond,
		float lifetime,
		float startSize,
		float endSize,
		DirectX::XMFLOAT4 startColor,
		DirectX::XMFLOAT4 endColor,
		DirectX::XMFLOAT3 startVelocity,
		DirectX::XMFLOAT3 velocityRandomRange,
		DirectX::XMFLOAT3 emitterPosition,
		DirectX::XMFLOAT3 positionRandomRange,
		DirectX::XMFLOAT4 rotationRandomRanges,
		DirectX::XMFLOAT3 emitterAcceleration,
		ID3D11Device* device,
		SimpleVertexShader* vs,
		SimplePixelShader* ps,
		ID3D11ShaderResourceView* texture
	);
	~Emitter();

	void Update(float dt, float currentTime);

	void UpdateSingleParticle(float currentTime, int index);
	void SpawnParticle(float currentTime);

	void Draw(ID3D11DeviceContext* context, Camera* camera, float currentTime);

private:
	// Emission properties
	int particlesPerSecond;
	float secondsPerParticle;
	float timeSinceEmit;

	int livingParticleCount;
	float lifetime;

	DirectX::XMFLOAT3 emitterAcceleration;
	DirectX::XMFLOAT3 emitterPosition;
	DirectX::XMFLOAT3 startVelocity;

	DirectX::XMFLOAT3 positionRandomRange;
	DirectX::XMFLOAT3 velocityRandomRange;
	DirectX::XMFLOAT4 rotationRandomRanges; // Min start, max start, min end, max end

	DirectX::XMFLOAT4 startColor;
	DirectX::XMFLOAT4 endColor;
	float startSize;
	float endSize;

	// Particle array
	Particle* particles;
	int maxParticles;
	int firstDeadIndex;
	int firstAliveIndex;

	// Rendering
	ID3D11Buffer* indexBuffer;

	ID3D11Buffer* particleDataBuffer;
	ID3D11ShaderResourceView* particleDataSRV;

	ID3D11ShaderResourceView* texture;
	SimpleVertexShader* vs;
	SimplePixelShader* ps;
};

