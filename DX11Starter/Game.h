#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "GameEntity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include "Emitter.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void DrawScene(float totalTime);
	void DrawFullscreenQuad(ID3D11ShaderResourceView* texture);
	void DrawRefraction();
	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
	DirectionaLight GetLight(DirectionaLight light, float lightAmount);
	


private:

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();
	
	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11ShaderResourceView* rockSRV;
	ID3D11ShaderResourceView* rockNormalSRV;
	ID3D11ShaderResourceView* fenceSRV;
	ID3D11SamplerState* samplerState;
	ID3D11RasterizerState* rasterState;
	ID3D11BlendState* rfBlendState;
	ID3D11DepthStencilState* depthState;
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	// Refraction stuff ------------------------
	// Render target view and SRV so we can render somewhere
	// other than the screen - necessary for refracting things
	// that are already drawn
	ID3D11SamplerState* refractSampler;
	ID3D11RenderTargetView* refractionRTV;
	ID3D11ShaderResourceView* refractionSRV;
	SimpleVertexShader* refractVS;
	SimplePixelShader* refractPS;
	SimpleVertexShader* quadVS;
	SimplePixelShader* quadPS;
	//Emitter stuff
	// Particle stuff
	ID3D11ShaderResourceView* particleTexture;
	SimpleVertexShader* particleVS;
	SimplePixelShader* particlePS;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;
	ID3D11RasterizerState* particleDebugRasterState;
	Emitter* emitter;
	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
	
	Mesh* g1;
	Mesh* g2;
	Mesh* g3;

	GameEntity* gameEntity1;
	GameEntity* gameEntity2;
	GameEntity* gameEntity3;
	GameEntity* gameEntity4;
	GameEntity* gameEntity5;
	std::vector<GameEntity*> entities;
	GameEntity* refractionEntity;
	Camera *camera1;
	Material* material1;
	Material* material2;
	Material* refractionMaterial;
	DirectionaLight dLight1;
	DirectionaLight dLight2;
	
};

