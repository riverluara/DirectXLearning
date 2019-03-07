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

	
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	samplerState->Release();
	clothSRV->Release();
	delete gameEntity1;
	delete gameEntity2;
	//delete gameEntity3;
	delete gameEntity4;
	delete gameEntity5;
	delete g1;
	delete g2;
	delete g3;
	delete camera1;

	delete material1;
	
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	CreateWICTextureFromFile(device, context, L"../../Textures/rock.jpg", 0, &rockSRV);
	CreateWICTextureFromFile(device, context, L"../../Textures/rockNormal.jpg", 0, &rockNormalSRV);
	// This sends data to GPU!!!
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&sampDesc, &samplerState);


	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

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

	
	pixelShader->SetFloat3("CameraPosition", XMFLOAT3(0, 0, -5)); // Matches camera view definition above
	
	

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
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };
	XMFLOAT2 uv = {0.0f, 0.0f};

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), normal, uv },

	};
	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	//Object2
	Vertex vertices2[] = 
	{ 
		{ XMFLOAT3(0.0f, +1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), normal, uv },
	};
	unsigned int indices2[] = { 0, 1, 2 };

	
	//Object3
	Vertex vertices3[] =
	{
		{ XMFLOAT3(-2.0f, +1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-0.5f, -1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-3.5f, -1.0f, +0.0f), normal, uv },
		{ XMFLOAT3(-2.0f, -1.5f, +0.0f), normal, uv },
	};
	unsigned int indices3[] = { 0, 1, 2, 1, 3, 2};
	
	//
	material1 = new Material(vertexShader, pixelShader, rockSRV, samplerState);
	//material2 = new Material(vertexShader, pixelShader, rockNormalSRV, samplerState);
	g1 = new Mesh("../../OBJ Files/cube.obj", device);

	gameEntity1 = new GameEntity(g1, material1);
	gameEntity2 = new GameEntity(g1, material1);
	//gameEntity3 = new GameEntity(g1, material1);
	g2 = new Mesh("../../OBJ Files/sphere.obj",device);
	gameEntity4 = new GameEntity(g2, material1);
	gameEntity5 = new GameEntity(g2, material1);
	g3 = new Mesh(vertices3, 4, indices3, 6, device);

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
	/*XMMATRIX trans = XMMatrixTranslation(0.0f, sinTime, 0.0f);
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(trans));*/
	gameEntity1->Scale(1.0f, 1.0f, 1.0f);
	gameEntity1->Rotate(0.0f, 0.0f, totalTime * 2.0f);
	gameEntity1->Move(sinTime, 0.0f, 1.0f);
	
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

	camera1->Update(deltaTime);
	
	


}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	//Set Point Light
	pixelShader->SetFloat3("PointLightPosition", XMFLOAT3(0, 5, 0));
	pixelShader->SetFloat3("PointLightColor", XMFLOAT3(0.5, 0.5, 0.5));

	pixelShader->SetFloat3("CameraPosition", camera1->GetCameraPosition()); // Matches camera view definition above

	
	//GameEntity1
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	
	gameEntity1->PrepareMaterial(camera1->GetViewMatrix(), camera1->GetProjectionMatrix());

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	gameEntity1->Draw(context);

	//GameEntity2
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	gameEntity2->PrepareMaterial(camera1->GetViewMatrix(), camera1->GetProjectionMatrix());

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.

	gameEntity2->Draw(context);
	//GameEntity3
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	//gameEntity3->PrepareMaterial(camera1->GetViewMatrix(), camera1->GetProjectionMatrix());
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	
	//gameEntity3->Draw(context);

		//GameEntity4
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	gameEntity4->PrepareMaterial(camera1->GetViewMatrix(), camera1->GetProjectionMatrix());

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	
	gameEntity4->Draw(context);

	//GameEntity5
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	gameEntity5->PrepareMaterial(camera1->GetViewMatrix(), camera1->GetProjectionMatrix());

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	
	gameEntity5->Draw(context);

	//


	//// Present the back buffer to the user
	////  - Puts the final frame we're drawing into the window so the user can see it
	////  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
	
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