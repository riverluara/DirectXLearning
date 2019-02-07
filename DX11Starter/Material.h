#pragma once
#include "SimpleShader.h"

class Material {
public:
	Material(SimpleVertexShader* vShader, SimplePixelShader* pShader);
	~Material();

private:
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
};