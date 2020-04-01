#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "primitiveclass.h"
#include "textureclass.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
using namespace DirectX;

//geo encapsulated
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct Transform
	{
		XMFLOAT3 position;
		XMFLOAT3 scale;
	};
public:
	ModelClass();
	ModelClass(XMFLOAT3 scale);
	ModelClass(const ModelClass&);
	~ModelClass();
	//bool Initialize(ID3D11Device*, LPCWSTR textureFilename);
	bool Initialize(ID3D11Device*, LPCWSTR,LPCWSTR);
	void Shutdown();
	void Frame();

	ID3D11ShaderResourceView* GetTexture();

	void Render(ID3D11DeviceContext*);
	int GetIndexCount();
	void Translate(XMFLOAT3 direction, float distance);
	void SetPosition(XMFLOAT3 new_position);
	void SetScale(XMFLOAT3 new_scale);
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetScale();
	bool Intersects(ModelClass* other, XMFLOAT3& intersectionTransform, HWND hwnd);

private:
	bool LoadTexture(ID3D11Device*, LPCWSTR);
	void ReleaseTexture();

	bool LoadModel(LPCWSTR);
	void ReleaseModel();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Device* device;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	TextureClass* m_Texture;
	ModelType* m_model;
public:
	int m_vertexCount, m_indexCount;
	unsigned long* indices;
	VertexType* vertices;
	Transform* transform;
};
#endif
