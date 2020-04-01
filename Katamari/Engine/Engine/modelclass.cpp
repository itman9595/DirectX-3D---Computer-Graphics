#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
	transform = new Transform();
	transform->position = XMFLOAT3(.0f, .0f, .0f);
}

ModelClass::ModelClass( XMFLOAT3 scale)
{
	ModelClass();
	transform = new Transform();
	transform->position = XMFLOAT3(.0f, .0f, .0f);
	transform->scale = scale;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

//The Initialize function will call the initialization functions for the vertexand index buffers.

bool ModelClass::Initialize(ID3D11Device* device, LPCWSTR modelFilename, LPCWSTR textureFilename)
{
	bool result;
	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}
	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}
	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}
	return true;
}

bool ModelClass::LoadTexture(ID3D11Device* device, LPCWSTR filename)
{
	bool result;
	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}
	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
bool ModelClass::LoadModel(LPCWSTR filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}
//The ReleaseModel function handles deleting the model data array.

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

//The Shutdown function will call the shutdown functions for the vertex and index buffers.
void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();
	// Release the vertex and index buffers.
	ShutdownBuffers();
	// Release the model data.
	//ReleaseModel();
	return;
}

void ModelClass::Frame()
{
	//update vertex buffer
}

//Render is called from the GraphicsClass::Render function.
//This function calls RenderBuffers to put the vertexand index buffers on 
//the graphics pipeline so the color shader will be able to render them.

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	Frame();
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

//GetIndexCount returns the number of indexes in the model.
//The color shader will need this information to draw this model.

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

void ModelClass::Translate(XMFLOAT3 direction, float distance)
{
	XMVECTOR v_offset = XMLoadFloat3(&direction) * distance;
	//now transform position stores real position of it's model
	// add it to all vertices
	XMFLOAT3 offset;
	XMStoreFloat3(&offset, v_offset);

	for (size_t i = 0; i < m_vertexCount; i++)
	{
		m_model[i].x += offset.x;
		m_model[i].y += offset.y;
		m_model[i].z += offset.z;
		m_model[i].tu += offset.x;
		m_model[i].tv += offset.y;
		m_model[i].nx += offset.x;
		m_model[i].ny += offset.y;
		m_model[i].nz += offset.z;
	}

	transform->position.x += offset.x;
	transform->position.y += offset.y;
	transform->position.z += offset.z;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];

	for (size_t i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
	}

	vertexData.pSysMem = vertices;
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer); // <--- memory leak!

	delete[] vertices;
	vertices = 0;
}

void ModelClass::SetPosition(XMFLOAT3 new_position)
{
	transform->position = new_position;
}

void ModelClass::SetScale(XMFLOAT3 new_scale)
{
	transform->scale = new_scale;
}

XMFLOAT3 ModelClass::GetPosition()
{
	return transform->position;
}

XMFLOAT3 ModelClass::GetScale()
{
	return transform->scale;
}

bool ModelClass::Intersects(ModelClass* other, XMFLOAT3& intersectionTransform, HWND hwnd)
{
	float distance = sqrt(pow(transform->position.x - (*other).transform->position.x, 2.0f) + pow(transform->position.y - (*other).transform->position.y, 2.0f) + pow(transform->position.z - (*other).transform->position.z, 2.0f));
	
	if (distance < 3.0f)
	{
		intersectionTransform.x = transform->position.x - (*other).transform->position.x;
		intersectionTransform.y = transform->position.y - (*other).transform->position.y;
		intersectionTransform.z = transform->position.z - (*other).transform->position.z;
		return true;
	}
	return false;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

//The InitializeBuffers function is where we handle creating the vertexand index buffers.
//Usually you would read in a modeland create the buffers from that data file.
//For this tutorial we will just set the points in the vertexand index buffer manually since it is only a single triangle.

bool ModelClass::InitializeBuffers(ID3D11Device* d11device)
{
	this->device = d11device;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//Load the vertex array and index array with data.
	for (size_t i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* ddeviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	ddeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// Set the index buffer to active in the input assembler so it can be rendered.
	ddeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	ddeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// Render the triangle.
	ddeviceContext->DrawIndexed(m_indexCount, 0, 0);
	return;
}
