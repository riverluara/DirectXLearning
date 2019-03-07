#pragma once

#include "DXCore.h"
#include "d3d11.h"
#include "SimpleShader.h"
#include <DirectXMath.h>
#include "Vertex.h"

class Mesh
{

public:
	Mesh(Vertex* vertices_1,
		int numVertices_1,
		unsigned int* indices_1,
		int numIndic_1,ID3D11Device* device_1);
	Mesh(char* file, ID3D11Device* device);
	~Mesh();
	

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();
	

private:

	int numIndices;//An integer specifying how many indices are in the mesh's index buffer
	

	// Buffers to hold actual geometry data
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	void CreateBuffer(Vertex* vertices_1,
		unsigned int* indices_1,
		int numIndice_1, ID3D11Device* device);
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	
};





