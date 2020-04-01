#include "graphicsclass.h"
#include "primitiveclass.h"
GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Ball = 0;
	m_Obj_1 = 0;
	m_Obj_2 = 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_hwnd = hwnd;

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if (!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.0f, -55.0f);

	m_Ball = new ModelClass;
	m_Ball->SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));
	// Create the Model.
	if (!m_Ball)
		return false;

	result = m_Ball->Initialize(m_D3D->GetDevice(), L"../Engine/sphere.txt", L"../Engine/checkerboard.tga");
	// Initialize the model.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}

	m_Obj_1 = new ModelClass;
	m_Obj_1->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	// Create the Model.
	if (!m_Obj_1)
		return false;
	result = m_Obj_1->Initialize(m_D3D->GetDevice(), L"../Engine/cube.txt", L"../Engine/brick.tga");

	// Initialize the model.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}
	m_Obj_1->Translate(XMFLOAT3(20.0f, 0.0f, 20.0f), 0.5f);

	m_Obj_2 = new ModelClass;
	m_Obj_2->SetScale(XMFLOAT3(1.0f, 1.0f, 1.0f));
	// Create the Model.
	if (!m_Obj_2)
		return false;
	result = m_Obj_2->Initialize(m_D3D->GetDevice(), L"../Engine/cube.txt", L"../Engine/brick.tga");

	// Initialize the model.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize model.", L"Error", MB_OK);
		return false;
	}
	m_Obj_2->Translate(XMFLOAT3(-20.0f, 0.0f, -20.0f), 0.5f);

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
		return false;

	// Initialize the texture shader object
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
		
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Ball)
	{
		m_Ball->Shutdown();
		delete m_Ball;
		m_Ball = 0;
	}

	if (m_Obj_1)
	{
		m_Obj_1->Shutdown();
		delete m_Obj_1;
		m_Obj_1 = 0;
	}

	if (m_Obj_2)
	{
		m_Obj_2->Shutdown();
		delete m_Obj_2;
		m_Obj_2 = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if (m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}
	return;
}

//perform update - it will be called when we start to press W/S or Up/Down buttons
bool GraphicsClass::Frame(int axisWS, int axisAD)
{
	bool result;
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)XM_PI * 0.001f;
	if (rotation > 360.0f)
		rotation -= 360.0f;
	
	// Render the graphics scene.
	result = Render(rotation);
	if (!result)
		return false;
	
	// Set the position of the camera.
	m_Camera->SetPosition(0.0f, 15.0f, -105.0f);

	XMFLOAT3 objPos;
	if (m_Ball->Intersects(m_Obj_1, objPos, m_hwnd))
	{
		intersected[0] = true;
		m_Obj_1->Translate(XMFLOAT3(objPos.x + 1.0f, 0.0f, objPos.z + 1.0f), 0.5f);
	}

	if (m_Ball->Intersects(m_Obj_2, objPos, m_hwnd))
	{
		intersected[1] = true;
		m_Obj_2->Translate(XMFLOAT3(objPos.x - 1.0f, 0.0f, objPos.z - 1.0f), 0.5f);
	}

	if (axisWS != 0 || axisAD != 0)
		m_Ball->Translate(XMFLOAT3(float(axisAD), 0.0f, float(axisWS)), 0.5f);

	if(intersected[0])
		m_Obj_1->Translate(XMFLOAT3(float(axisAD), 0.0f, float(axisWS)), 0.5f);

	if (intersected[1])
		m_Obj_2->Translate(XMFLOAT3(float(axisAD), 0.0f, float(axisWS)), 0.5f);

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	XMMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	
	XMFLOAT3 m_Ball_Scale = m_Ball->GetScale();
	worldMatrix = XMMatrixScaling(3.0f, 3.0f, 3.0f);
	m_Ball->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Ball->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_Ball->GetTexture());

	m_Obj_1->Render(m_D3D->GetDeviceContext());
	
	m_Obj_2->Render(m_D3D->GetDeviceContext());
	
	if (!result)
		return false;

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}
