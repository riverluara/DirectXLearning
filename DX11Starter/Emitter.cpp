#include "Emitter.h"

using namespace DirectX;

Emitter::Emitter(
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
)
{
	// Save params
	this->vs = vs;
	this->ps = ps;
	this->texture = texture;

	this->maxParticles = maxParticles;
	this->lifetime = lifetime;
	this->startColor = startColor;
	this->endColor = endColor;
	this->startVelocity = startVelocity;
	this->startSize = startSize;
	this->endSize = endSize;
	this->particlesPerSecond = particlesPerSecond;
	this->secondsPerParticle = 1.0f / particlesPerSecond;

	this->velocityRandomRange = velocityRandomRange;
	this->positionRandomRange = positionRandomRange;
	this->rotationRandomRanges = rotationRandomRanges;

	this->emitterPosition = emitterPosition;
	this->emitterAcceleration = emitterAcceleration;

	timeSinceEmit = 0;
	livingParticleCount = 0;
	firstAliveIndex = 0;
	firstDeadIndex = 0;

	// Make the particle array
	particles = new Particle[maxParticles];
	ZeroMemory(particles, sizeof(Particle) * maxParticles);

	// Create buffers for drawing particles

	// Index buffer data
	unsigned int* indices = new unsigned int[maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular (static) index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * maxParticles * 6;
	device->CreateBuffer(&ibDesc, &indexData, &indexBuffer);

	// Just make a single buffer to hold copy of all particle data
	D3D11_BUFFER_DESC allParticleBufferDesc = {};
	allParticleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	allParticleBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	allParticleBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	allParticleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	allParticleBufferDesc.StructureByteStride = sizeof(Particle);
	allParticleBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
	device->CreateBuffer(&allParticleBufferDesc, 0, &particleDataBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	device->CreateShaderResourceView(particleDataBuffer, &srvDesc, &particleDataSRV);

	delete[] indices;
}


Emitter::~Emitter()
{
	delete[] particles;
	indexBuffer->Release();

	particleDataBuffer->Release();
	particleDataSRV->Release();
}

void Emitter::Update(float dt, float currentTime)
{
	// Anything to update?
	if (livingParticleCount > 0)
	{
		// Update all particles - Check cyclic buffer first
		if (firstAliveIndex < firstDeadIndex)
		{
			// First alive is BEFORE first dead, so the "living" particles are contiguous
			// 
			// 0 -------- FIRST ALIVE ----------- FIRST DEAD -------- MAX
			// |    dead    |            alive       |         dead    |

			// First alive is before first dead, so no wrapping
			for (int i = firstAliveIndex; i < firstDeadIndex; i++)
				UpdateSingleParticle(currentTime, i);
		}
		else if (firstDeadIndex < firstAliveIndex)
		{
			// First alive is AFTER first dead, so the "living" particles wrap around
			// 
			// 0 -------- FIRST DEAD ----------- FIRST ALIVE -------- MAX
			// |    alive    |            dead       |         alive   |

			// Update first half (from firstAlive to max particles)
			for (int i = firstAliveIndex; i < maxParticles; i++)
				UpdateSingleParticle(currentTime, i);

			// Update second half (from 0 to first dead)
			for (int i = 0; i < firstDeadIndex; i++)
				UpdateSingleParticle(currentTime, i);
		}
		else
		{
			// First alive is EQUAL TO first dead, so they're either all alive or all dead
			// - Since we know at least one is alive, they should all be
			//
			//            FIRST ALIVE
			// 0 -------- FIRST DEAD -------------------------------- MAX
			// |    alive     |                   alive                |
			for (int i = 0; i < maxParticles; i++)
				UpdateSingleParticle(currentTime, i);
		}
	}

	// Add to the time
	timeSinceEmit += dt;

	// Enough time to emit?
	while (timeSinceEmit > secondsPerParticle)
	{
		SpawnParticle(currentTime);
		timeSinceEmit -= secondsPerParticle;
	}
}

void Emitter::UpdateSingleParticle(float currentTime, int index)
{
	float age = currentTime - particles[index].SpawnTime;

	// Update and check for death
	if (age >= lifetime)
	{
		// Recent death, so retire by moving alive count
		firstAliveIndex++;
		firstAliveIndex %= maxParticles;
		livingParticleCount--;
		return;
	}
}

void Emitter::SpawnParticle(float currentTime)
{
	// Any left to spawn?
	if (livingParticleCount == maxParticles)
		return;

	// Reset the first dead particle
	particles[firstDeadIndex].SpawnTime = currentTime;

	particles[firstDeadIndex].StartPosition = emitterPosition;
	particles[firstDeadIndex].StartPosition.x += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.x;
	particles[firstDeadIndex].StartPosition.y += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.y;
	particles[firstDeadIndex].StartPosition.z += (((float)rand() / RAND_MAX) * 2 - 1) * positionRandomRange.z;

	particles[firstDeadIndex].StartVelocity = startVelocity;
	particles[firstDeadIndex].StartVelocity.x += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.x;
	particles[firstDeadIndex].StartVelocity.y += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.y;
	particles[firstDeadIndex].StartVelocity.z += (((float)rand() / RAND_MAX) * 2 - 1) * velocityRandomRange.z;

	float rotStartMin = rotationRandomRanges.x;
	float rotStartMax = rotationRandomRanges.y;
	particles[firstDeadIndex].RotationStart = ((float)rand() / RAND_MAX) * (rotStartMax - rotStartMin) + rotStartMin;

	float rotEndMin = rotationRandomRanges.z;
	float rotEndMax = rotationRandomRanges.w;
	particles[firstDeadIndex].RotationEnd = ((float)rand() / RAND_MAX) * (rotEndMax - rotEndMin) + rotEndMin;

	// Increment and wrap
	firstDeadIndex++;
	firstDeadIndex %= maxParticles;

	livingParticleCount++;
}


void Emitter::Draw(ID3D11DeviceContext* context, Camera* camera, float currentTime)
{
	// Do a raw copy of the particle data to the dynamic structured buffer
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(particleDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, particles, sizeof(Particle) * maxParticles);
	context->Unmap(particleDataBuffer, 0);

	// Set up buffers
	UINT stride = 0;
	UINT offset = 0;
	ID3D11Buffer* nullBuffer = 0;
	context->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);


	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());

	vs->SetFloat3("acceleration", emitterAcceleration);
	vs->SetFloat4("startColor", startColor);
	vs->SetFloat4("endColor", endColor);
	vs->SetFloat("startSize", startSize);
	vs->SetFloat("endSize", endSize);
	vs->SetFloat("lifetime", lifetime);
	vs->SetFloat("currentTime", currentTime);

	vs->SetShader();

	// Do it manually, as simple shader doesn't currently handle
	// setting structured buffers for a vertex shader
	context->VSSetShaderResources(0, 1, &particleDataSRV);

	ps->SetShaderResourceView("particle", texture);
	ps->SetShader();


	if (firstAliveIndex < firstDeadIndex)
	{
		// Draw from (firstAliveIndex -> firstDeadIndex)
		vs->SetInt("startIndex", firstAliveIndex);
		vs->CopyAllBufferData();
		context->DrawIndexed(livingParticleCount * 6, 0, 0);
	}
	else
	{
		// Draw first half (0 -> dead)
		vs->SetInt("startIndex", 0);
		vs->CopyAllBufferData();
		context->DrawIndexed(firstDeadIndex * 6, 0, 0);

		// Draw second half (alive -> max)
		vs->SetInt("startIndex", firstAliveIndex);
		vs->CopyAllBufferData();
		context->DrawIndexed((maxParticles - firstAliveIndex) * 6, 0, 0);
	}

}
