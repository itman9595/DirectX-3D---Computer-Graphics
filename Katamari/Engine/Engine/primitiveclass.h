#ifndef _PRIMITIVECLASS_H_
#define _PRIMITIVECLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

static enum class PrimitiveType { Triangle, Rectangle, Cube};
class PrimitiveClass 
{
public: 
	int vertexCount, indexCount;
	unsigned long* indices;
	struct ModelData
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	ModelData* vertices;
	PrimitiveClass(PrimitiveType type);
	~PrimitiveClass();
	void Rescale(float x, float y, float z);
	void SetPosition(float x,float y, float z);
private :
	void BuildTriangle();
	void BuildRectangle();
};
#endif