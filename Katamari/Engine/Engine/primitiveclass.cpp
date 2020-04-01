#include "primitiveclass.h"

PrimitiveClass::PrimitiveClass(PrimitiveType type)
{
	switch (type)
	{
	case PrimitiveType::Triangle:BuildTriangle();
		break;
	case PrimitiveType::Rectangle:BuildRectangle();
		break;
	default:
		break;
	}
}

PrimitiveClass::~PrimitiveClass()
{
}

void PrimitiveClass::Rescale(float x, float y, float z)
{
	for (size_t i = 0; i < vertexCount; i++)
	{
		vertices[i].position.x *= x;
		vertices[i].position.y *= y;
		vertices[i].position.z *= z;
	}
}

void PrimitiveClass::SetPosition(float x, float y, float z)
{
	for (size_t i = 0; i < vertexCount; i++)
	{
		vertices[i].position.x += x;
		vertices[i].position.y += y;
		vertices[i].position.z += z;
	}
}

void PrimitiveClass::BuildTriangle()
{
	this->vertexCount = 3;
	this->indexCount = 3;
	vertices = new ModelData[vertexCount];
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);
	vertices[0].texture = XMFLOAT2(0.0f, 1.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);
	vertices[1].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);
	vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);


	indices = new unsigned long[indexCount];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
}

void PrimitiveClass::BuildRectangle() 
{
	this->vertexCount = 4;
	this->indexCount = 6;
	vertices = new ModelData[vertexCount];
	vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f);
	vertices[1].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);
	vertices[2].position = XMFLOAT3(0.5f, 0.5f, 0.0f);
	vertices[3].position = XMFLOAT3(0.5f, -0.5f, 0.0f);

	indices = new unsigned long[indexCount];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;
}

