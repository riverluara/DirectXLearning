  #include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		// The application's handle
		"DirectX Game",	   	// Text for the window's title bar
		1280,			// Width of the window's client area
		720,			// Height of the window's client area
		true)			// Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexBuffer = 0;
	indexBuffer = 0;
	vertexShader = 0;
	pixelShader = 0;
	/*refractVS = 0;
	refractPS = 0;
	quadVS = 0;
	quadPS = 0;*/

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class
	if (vertexShader) {
		delete vertexShader;
	}
	if (pixelShader) {
		delete pixelShader;
	}
	
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	samplerState->Release();
	rockSRV->Release();
	rockNormalSRV->Release();
	fenceSRV->Release();
	//depthState->Release();
	//blendState->Release();
	//rasterState->Release();
	//for (auto& e : entities) delete e;
	
	
	delete gameEntity1;
	
	delete gameEntity2;
	//delete gameEntity3;
	delete gameEntity4;
	delete gameEntity5;
	delete g1;
	delete g2;
	//delete g3;
	delete camera1;
	
	delete material1;
	delete material2;
	
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{

	CreateWICTextureFromFile(device, context, L"../../Textures/rock.jpg", 0, &rockSRV);
	CreateWICTextureFromFile(device, context, L"../../Textures/rockNormals.jpg", 0, &rockNormalSRV);
	CreateWICTextureFromFile(device, context, L"../../Textures/IceTexture.png", 0, &fenceSRV);
	CreateWICTextureFromFile(device, context, L"../../Textures/multiplesnow.png", 0, &particleTexture);
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, &samplerState);
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

	// This sends data to GPU!!!
	



	

	
	pixelShader->SetFloat3("CameraPosition", XMFLOAT3(0, 0, -5)); // Matches camera view definition above
	
	//Add depth, so can render without blending
	//Add rasterize desc, depth stencil desc, blend desc
	//Add a column of balls , remeber to delete them
	//No need to change pixel shader or vertex shader

	

	// A depth state for the particles
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // Turns off depth writing
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	device->CreateDepthStencilState(&dsDesc, &particleDepthState);

	//// Blend for particles (additive)
	D3D11_BLEND_DESC blend = {};
	blend.AlphaToCoverageEnable = false;
	blend.IndependentBlendEnable = false;
	blend.RenderTarget[0].BlendEnable = true;
	blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Still respect pixel shader output alpha
	blend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blend, &particleBlendState);

	
	// Debug rasterizer state for particles
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_BACK;
	rd.DepthClipEnable = true;
	rd.FillMode = D3D11_FILL_WIREFRAME;
	device->CreateRasterizerState(&rd, &particleDebugRasterState);

	emitter = new Emitter(
		200,							// Max particles
		30,								// Particles per second
		3,								// Particle lifetime
		0.75f,							// Start size
		0.5f,							// End size
		XMFLOAT4(0.8f, 0.8f, 0.8f, 0.3f),// Start color
		XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f),// End color
		XMFLOAT3(0, -2.0f, 0),				// Start velocity
		XMFLOAT3(0.5f, 0.1f, 0.1f),		// Velocity randomness range
		XMFLOAT3(-2.5f, 2.0f, 0.0),			// Emitter position
		XMFLOAT3(6, 0, 0),				// Position randomness range
		XMFLOAT4(0, 0, -3, 3),			// Random rotation ranges (startMin, startMax, endMin, endMax)
		XMFLOAT3(0, -1, 0),				// Constant acceleration
		device,
		particleVS,
		particlePS,
		particleTexture);

	//Refraction setup
	ID3D11Texture2D* refractionRenderTexture;

	//Set up render texture
	D3D11_TEXTURE2D_DESC rtDesc = {};
	rtDesc.Width = width;
	rtDesc.Height = height;
	rtDesc.MipLevels = 1;
	rtDesc.ArraySize = 1;
	rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtDesc.Usage = D3D11_USAGE_DEFAULT;
	rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rtDesc.CPUAccessFlags = 0;
	rtDesc.MiscFlags = 0;
	rtDesc.SampleDesc.Count = 1;
	rtDesc.SampleDesc.Quality = 0;
	device->CreateTexture2D(&rtDesc, 0, &refractionRenderTexture);
	// Set up render target view
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = rtDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;
	device->CreateRenderTargetView(refractionRenderTexture, &rtvDesc, &refractionRTV);
	// Set up shader resource view for same texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = rtDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(refractionRenderTexture, &srvDesc, &refractionSRV);

	// All done with this texture ref
	refractionRenderTexture->Release();
	// Set up a sampler that uses clamp addressing
	// for use when doing refration - this is useful so 
	// that we don't wrap the refraction from the other
	// side of the screen
	D3D11_SAMPLER_DESC rSamp = {};
	rSamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	rSamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	rSamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	rSamp.Filter = D3D11_FILTER_ANISOTROPIC;
	rSamp.MaxAnisotropy = 16;
	rSamp.MaxLOD = D3D11_FLOAT32_MAX;

	// Ask DirectX for the actual object
	device->CreateSamplerState(&rSamp, &refractSampler);

	D3D11_BLEND_DESC bd = {};
	bd.RenderTarget[0].BlendEnable = true;

	// Settings for blending RGB channels
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	// Settings for blending alpha channel
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	// Setting for masking out individual color channels
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// Create the state
	device->CreateBlendState(&bd, &rfBlendState);




	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	vertexShader->LoadShaderFile(L"VertexShader.cso");

	pixelShader = new SimplePixelShader(device, context);
	pixelShader->LoadShaderFile(L"PixelShader.cso");
	// Refraction shaders
	quadVS = new SimpleVertexShader(device, context);
	quadVS->LoadShaderFile(L"FullscreenQuadVS.cso");

	quadPS = new SimplePixelShader(device, context);
	quadPS->LoadShaderFile(L"FullscreenQuadPS.cso");

	refractVS = new SimpleVertexShader(device, context);
	refractVS->LoadShaderFile(L"RefractVS.cso");

	refractPS = new SimplePixelShader(device, context);
	refractPS->LoadShaderFile(L"RefractPS.cso");

	particleVS = new SimpleVertexShader(device, context);
	particleVS->LoadShaderFile(L"ParticleVS.cso");

	particlePS = new SimplePixelShader(device, context);
	particlePS->LoadShaderFile(L"ParticlePS.cso");
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//    update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//    an identity matrix.  This is just to show that HLSL expects a different
	//    matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------


void Game::CreateBasicGeometry()
{
	
	
	//
	material1 = new Material(vertexShader, pixelShader, rockSRV, rockNormalSRV, samplerState);
	 material2 = new Material(vertexShader, pixelShader, fenceSRV, fenceSRV, samplerState);
	 refractionMaterial = new Material(refractVS, refractPS, refractionSRV, refractionSRV, refractSampler);
	//material2 = new Material(vertexShader, pixelShader, rockNormalSRV, samplerState);
	g1 = new Mesh("../../OBJ Files/sphere.obj", device);
	g2 = new Mesh("../../OBJ Files/cube.obj", device);
	gameEntity1 = new GameEntity(g1, material1);
	gameEntity2 = new GameEntity(g1, material1);
	//gameEntity3 = new GameEntity(g1, material1);
	
	gameEntity4 = new GameEntity(g2, material1);
	gameEntity5 = new GameEntity(g2, material1);
	refractionEntity = new GameEntity(g1,refractionMaterial);
	//g3 = new Mesh(vertices3, 4, indices3, 6, device);
	//GameEntity* ge = new GameEntity(g1, material1);
	//GameEntity* geFence = new GameEntity(g1, material2);
	//Add different game entities with different materials and meshes
	//entities.push_back(ge);
	//entities.push_back(geFence);
	camera1 = new Camera(width, height);
	
	
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera1->UpdateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
	float sinTime = sin(totalTime * 2);
	emitter->Update(deltaTime, totalTime);
	/*XMMATRIX trans = XMMatrixTranslation(0.0f, sinTime, 0.0f);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(trans));*/
	/*gameEntity1->Scale(1.0f, 1.0f, 1.0f);
	gameEntity1->Rotate(0.0f, 0.0f, totalTime * 2.0f);
	gameEntity1->Move(sinTime, 0.0f, 1.0f);
	*/
	gameEntity2->Move(0.0f, 0.0f, 3.0f);
	gameEntity2->Scale(2.5f, 2.5f, 2.5f);
	gameEntity2->Rotate(0.0f, 0.0f, totalTime * 2.0f + 60.0f);
	gameEntity2->Move(sinTime, 0.0f, 5.0f);

	//gameEntity3->Move(0.0f, 5.0f, 10.0f);
	//gameEntity3->Scale(abs(1.8f * sinTime),abs(1.8f * sinTime), 1.0f);
    //gameEntity3->Rotate(0.0f, 0.0f, totalTime*2.0f);
	//gameEntity3->Move(sinTime, 0.0f, 2.0f);

	//gameEntity4->Scale(0.5f, 0.5f, 1.0f);
	gameEntity4->Move(-2.8f * sinTime,0.0f , -1.0f);

	//gameEntity5->Scale(0.5f, 0.5f, 1.0f);
	gameEntity5->Move(2.8f * sinTime, 0.0f, -1.0f);
	//gameEntity5->Scale(0.75f * sinTime + 0.75f, 0.75f * sinTime + 0.75f, 1.0f);
	refractionEntity->Move( sinTime, 0.0f, -1.0f);
	camera1->Update(deltaTime);
	
	


}
void Game::DrawScene(float totalTime) {
	
	dLight1.AmbientColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	dLight1.DiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	dLight1.Direction = { 1.0f, -1.0f, 0.0f };

	pixelShader->SetData("dLight1", &dLight1, sizeof(DirectionaLight));
	//pixelShader->CopyAllBufferData();

	dLight2.AmbientColor = { 0.1f, 0.1f, 0.5f, 1.0f };
	dLight2.DiffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	dLight2.Direction = { -0.50f, 1.0f, 0.0f };
	pixelShader->SetData("dLight2", &dLight2, sizeof(DirectionaLight));
	//pixelShader->CopyAllBufferData();
	//Set Point Light
	pixelShader->SetFloat3("PointLightPosition", XMFLOAT3(0, 5, 0));
	pixelShader->SetFloat3("PointLightColor", XMFLOAT3(0.5, 0.5, 0.5));
	pixelShader->SetFloat3("DirLightColor", XMFLOAT3(0.8f, 0.8f, 0.8f));
	pixelShader->SetFloat3("CameraPosition", camera1->GetCameraPosition()); 
	pixelShader->CopyAllBufferData() ;// Matches camera view definition above
	gameEntity1->PrepareMaterial("sampState", "DiffuseTexture", "NormalTexture", camera1->GetViewMatrix(), camera1->GetProjectionMatrix());
	
	// Draw multiple of the same object
	for (int i = 0; i < 5; i++)
	{
		// Load, un-transpose, translate, re-transpose, store
		XMMATRIX wMat = XMMatrixTranspose(XMLoadFloat4x4(gameEntity1->GetWorldMatrix()));
		XMFLOAT4X4 w;
		XMStoreFloat4x4(&w, XMMatrixTranspose(wMat * XMMatrixTranslation(i * (-1.0f), 0, 0)));

		// Changes per object
		vertexShader->SetMatrix4x4("world", w);

		// This is a little sloppy, but just for the demo, copy everything
		vertexShader->CopyAllBufferData();
		// Finally do the actual drawing
		gameEntity1->Draw(context);
	}
	
	// Particle states
	
}
void Game::DrawFullscreenQuad(ID3D11ShaderResourceView* texture) {
	// First, turn off our buffers, as we'll be generating the vertex
	// data on the fly in a special vertex shader using the index of each vert
	context->IASetVertexBuffers(0, 0, 0, 0, 0);
	context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
	// Set up the fullscreen quad shaders
	quadVS->SetShader();

	quadPS->SetShaderResourceView("Pixels", texture);
	quadPS->SetSamplerState("Sampler", samplerState);
	quadPS->SetShader();

	// Draw
	context->Draw(3, 0);
}
void Game::DrawRefraction() {
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* vb = refractionEntity->GetMeshVertexBuffer();
	ID3D11Buffer* ib = refractionEntity->GetMeshIndexBuffer();
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	// Setup vertex shader
	XMMATRIX wMat = XMMatrixTranspose(XMLoadFloat4x4(refractionEntity->GetWorldMatrix()));
	XMFLOAT4X4 w;
	XMStoreFloat4x4(&w, XMMatrixTranspose(wMat * XMMatrixTranslation(-0.5f, 0, -1.0f)));

	
	refractVS->SetMatrix4x4("world", w);
	refractVS->SetMatrix4x4("view", camera1->GetViewMatrix());
	refractVS->SetMatrix4x4("projection", camera1->GetProjectionMatrix());
	refractVS->CopyAllBufferData();
	refractVS->SetShader();

	// Setup pixel shader
	refractPS->SetShaderResourceView("ScenePixels", refractionSRV);	// Pixels of the screen
	refractPS->SetShaderResourceView("NormalMap", rockNormalSRV);	// Normal map for the object itself
	refractPS->SetSamplerState("BasicSampler", samplerState);			// Sampler for the normal map
	refractPS->SetSamplerState("RefractSampler", refractSampler);	// Uses CLAMP on the edges
	refractPS->SetFloat3("CameraPosition", camera1->GetCameraPosition());
	refractPS->SetMatrix4x4("view", camera1->GetViewMatrix());				// View matrix, so we can put normals into view space
	refractPS->CopyAllBufferData();
	refractPS->SetShader();

	// Finally do the actual drawing
	refractionEntity->Draw(context);
}
// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	
	// Background color (black in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear any and all render targets we intend to use, and the depth buffer
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearRenderTargetView(refractionRTV, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	
	// Use our refraction render target and our regular depth buffer
	context->OMSetRenderTargets(1, &refractionRTV, depthStencilView);

	//// Draw the scene (WITHOUT the refracting object)
	DrawScene(totalTime);
	
	// Back to the screen, but NO depth buffer for now!
	// We just need to plaster the pixels from the render target onto the 
	// screen without affecting (or respecting) the existing depth buffer
	context->OMSetRenderTargets(1, &backBufferRTV, 0);

	// Draw a fullscreen quad with our render target texture (so the user can see
	// what we've drawn so far).  
	DrawFullscreenQuad(refractionSRV);

	// Turn the depth buffer back on, so we can still
	// used the depths from our earlier scene render
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	context->OMSetBlendState(rfBlendState, 0, 0xFFFFFFFF);
	// Draw the refraction object
	DrawRefraction();
	
	
	// Unbind all textures at the end of the frame
	// This is a good idea any time we're using extra render targets
	// that we intend to sample from on the next frame
	ID3D11ShaderResourceView* nullSRV[16] = {};
	context->PSSetShaderResources(0, 16, nullSRV);

	
	

	
	float blend[4] = { 1,1,1,1 }; 
	context->OMSetBlendState(particleBlendState, blend, 0xffffffff);	// Additive blending
	context->OMSetDepthStencilState(particleDepthState, 0);				// No depth WRITING
	// No wireframe debug
	particlePS->SetInt("debugWireframe", 0);
	particlePS->CopyAllBufferData();

	// Draw the emitter
	emitter->Draw(context, camera1, totalTime);

	context->OMSetBlendState(0, blend, 0xffffffff);
	context->OMSetDepthStencilState(0, 0);
	context->RSSetState(0);
	//// Present the back buffer to the user
	////  - Puts the final frame we're drawing into the window so the user can see it
	////  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
	// Must re-bind after Present() due to swap chain options
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	;


	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (buttonState & 0x0001) {

		float distanceX = float(x - prevMousePos.x);
		float distanceY = float(y - prevMousePos.y);
		float rotateSpeed = 0.001f;
		camera1->SetCameraRotation(distanceX * rotateSpeed, distanceY * rotateSpeed);
		
	}
	
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
DirectionaLight Game::GetLight(DirectionaLight light,float lightAmount) {
	DirectX::XMVECTOR direction;
	direction = DirectX::XMLoadFloat3(&light.Direction);
	direction = DirectX::XMVector3Normalize(direction * (-1.0f));
	DirectX::XMStoreFloat3(&light.Direction, direction);
	return light;


}
#pragma endregion