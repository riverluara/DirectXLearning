#include "Material.h"
Material::Material(SimpleVertexShader* vShader, SimplePixelShader* pShader) {
	vertexShader = vShader;
	pixelShader = pShader;
}
Material::~Material() {
	
	delete vertexShader;
	delete pixelShader;
}