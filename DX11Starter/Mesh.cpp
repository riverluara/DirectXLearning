#include "Mesh.h"
#include "Vertex.h"


Mesh::Mesh(Vertex* vertices_1,
	int numVertices_1,
	unsigned int* indices_1,
	int numIndice_1,ID3D11Device* device) {

	// Create the VERTEX BUFFER description -----------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * numVertices_1;       // 3 = number of vertices in the buffer
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Tells DirectX this is a vertex buffer
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	// Create the proper struct to hold the initial vertex data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialVertexData1;
	initialVertexData1.pSysMem = vertices_1;
	device->CreateBuffer(&vbd, &initialVertexData1, &vertexBuffer);

	// Create the INDEX BUFFER description ------------------------------------
	// - The description is created on the stack because we only need
	//    it to create the buffer.  The description is then useless.
	numIndices = numIndice_1;
	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(int) * numIndice_1;         // 3 = number of indices in the buffer
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // Tells DirectX this is an index buffer
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	// Create the proper struct to hold the initial index data
	// - This is how we put the initial data into the buffer
	D3D11_SUBRESOURCE_DATA initialIndexData1;
	initialIndexData1.pSysMem = indices_1;
	
	// Actually create the buffer with the initial data
	// - Once we do this, we'll NEVER CHANGE THE BUFFER AGAIN
	device->CreateBuffer(&ibd, &initialIndexData1, &indexBuffer);
}

Mesh::~Mesh(void) {
	if (vertexBuffer) { vertexBuffer->Release(); }
	if (indexBuffer) { indexBuffer->Release(); }
}

ID3D11Buffer* Mesh::GetIndexBuffer() {
	return indexBuffer;
}

ID3D11Buffer* Mesh::GetVertexBuffer() {
	return vertexBuffer;
}

int Mesh::GetIndexCount() {
	return numIndices;
}
