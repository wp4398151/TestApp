#include "stdafx.h"
#include "DirectXWindow.h"

bool MyCopyResource(ID3D10Texture2D **pTargetRenderTexture,
	ID3D10Texture2D *pSrcRenderTexture, ID3D10Device* pDevice, LPCWSTR filePath);

struct SimpleVertex
{
	D3DXVECTOR3 Pos;
	D3DXVECTOR2 Tex;
};

char CDirectXWindow::s_DrawTextureP[] =
"/* PS */                                                                       \r\n"
"uniform Texture2D diffuseTexture;                                              \r\n"
"uniform float4 outputColor;                                                    \r\n"
"                                                                               \r\n"
"SamplerState textureSampler                                                    \r\n"
"{                                                                              \r\n"
"    AddressU  = Clamp;                                                         \r\n"
"    AddressV  = Clamp;                                                         \r\n"
"    Filter    = Linear;                                                        \r\n"
"};                                                                             \r\n"
"                                                                               \r\n"
"struct VertData                                                                \r\n"
"{                                                                              \r\n"
"    float4 pos      : SV_Position;                                             \r\n"
"    float2 texCoord : TexCoord0;                                               \r\n"
"};                                                                             \r\n"
"                                                                               \r\n"
"float4 main(VertData input) : SV_Target                                        \r\n"
"{                                                                              \r\n"
"    return diffuseTexture.Sample(textureSampler, input.texCoord) * outputColor;\r\n"
"}                                                                              \r\n";

char CDirectXWindow::s_DrawTextureV[] =
"/* VS */                                                                       \r\n"
"uniform float4x4 ViewProj;                                                     \r\n"
"                                                                               \r\n"
"struct VertData                                                                \r\n"
"{                                                                              \r\n"
"    float4 pos      : SV_Position;                                             \r\n"
"    float2 texCoord : TexCoord0;                                               \r\n"
"};                                                                             \r\n"
"                                                                               \r\n"
"VertData main(VertData input)                                                  \r\n"
"{                                                                              \r\n"
"    VertData output;                                                           \r\n"
"                                                                               \r\n"
"    output.pos = mul(float4(input.pos.xyz, 1.0), ViewProj);                    \r\n"
"    output.texCoord = input.texCoord;                                          \r\n"
"                                                                               \r\n"
"    return output;                                                             \r\n"
"}                                                                              \r\n";

char CDirectXWindow::s_strEffect[] =
"uniform Texture2D txDiffuse;													\r\n"
"SamplerState samLinear													\r\n"
"{																		\r\n"
"	Filter = MIN_MAG_MIP_LINEAR;													\r\n"
"	AddressU = Wrap;													\r\n"
"	AddressV = Wrap;													\r\n"
"};																		\r\n"
"uniform float4 outputColor;                                                    \r\n"
"uniform matrix World;															\r\n"
"uniform matrix View;															\r\n"
"uniform matrix Projection;														\r\n"
"																		\r\n"
"struct VS_INPUT														\r\n"
"{																		\r\n"
"	float4 Pos : POSITION;												\r\n"
"	float2 Tex : TEXCOORD0;												\r\n"
"};																		\r\n"
"struct PS_INPUT														\r\n"
"{																		\r\n"
"	float4 Pos : SV_POSITION;											\r\n"
"	float2 Tex : TEXCOORD0;  											\r\n"
"};																		\r\n"
"PS_INPUT VS(VS_INPUT input)											\r\n"
"{																		\r\n"
"	PS_INPUT output = (PS_INPUT)0;										\r\n"
"	output.Pos = mul(input.Pos, World);									\r\n"
"	output.Pos = mul(output.Pos, View);									\r\n"
"	output.Pos = mul(output.Pos, Projection);							\r\n"
"	output.Tex = input.Tex;												\r\n"
"	return output;														\r\n"
"}																		\r\n"
"																		\r\n"
"float4 PS(PS_INPUT input) : SV_Target									\r\n"
"{																		\r\n"
"	return txDiffuse.Sample(samLinear, input.Tex);						\r\n"
"}																		\r\n"
"																		\r\n"
"technique10 Render														\r\n"
"{																		\r\n"
"	pass P0																\r\n"
"	{																	\r\n"
"		SetVertexShader(CompileShader(vs_4_0, VS()));					\r\n"
"		SetGeometryShader(NULL);										\r\n"
"		SetPixelShader(CompileShader(ps_4_0, PS()));					\r\n"
"	}																	\r\n"
"}																		\r\n";

CDirectXWindow::CDirectXWindow()
{
	m_hwnd = NULL;
	m_rcClipRect = RECT{ 0, 0, 0, 0 };
	m_pSwapChain = NULL;
	m_pd3dDevice = NULL;
	m_pRenderTargetView = NULL;
	m_pFrameResource = NULL;
	m_pEffect = NULL;
	m_pTechnique = NULL;
	m_pVertexLayout = NULL;
	m_pVertexBuffer = NULL;
	m_pWorldVariable = NULL;
	m_pViewVariable = NULL;
	m_pProjectionVariable = NULL;
	m_pTextureRV = NULL;
}

CDirectXWindow::~CDirectXWindow()
{
	Clear();
}

void CDirectXWindow::Clear()
{
	if (m_pd3dDevice) m_pd3dDevice->ClearState();
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pTextureRV);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pRenderTargetView);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pFrameResource);
}

void CDirectXWindow::CopyFromSwapBackbuffer()
{
	// Create a render target view
	ID3D10Texture2D *pBackBuffer = NULL;
	HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "m_pSwapChain->GetBuffer failed ! :" << hr;
		return ;
	}

	ID3D10Texture2D *pTargetRenderTexture = NULL;
	MyCopyResource(&pTargetRenderTexture, pBackBuffer, m_pd3dDevice, L"C:\\testbmp\\BackBuffer\\");
	SAFE_RELEASE(pTargetRenderTexture);
	return ;
}

bool CDirectXWindow::MapTextrueFromFrame(ID3D10Texture2D* pTex2D)
{
	D3D10_SHADER_RESOURCE_VIEW_DESC resourceDesc;
	ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	resourceDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	resourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	resourceDesc.Texture2D.MipLevels = 1;

	HRESULT err = m_pd3dDevice->CreateShaderResourceView(pTex2D, &resourceDesc, &m_pFrameResource);
	if (!SUCCEEDED(err))
	{
		SAFE_RELEASE(pTex2D);
		LOG(ERROR) << "D3D10Texture::CreateFromSharedHandle: CreateShaderResourceView failed, result = " << err;
		return false;
	}

	m_pd3dDevice->PSSetShaderResources(0, 1, &m_pFrameResource);
	m_pDiffuseVariable = m_pEffect->GetVariableByName("txDiffuse")->AsShaderResource();
	return true;
}

bool CDirectXWindow::MapTextrue()
{
	HRESULT hr = D3DX10CreateShaderResourceViewFromFile(m_pd3dDevice, 
					L"seafloor.dds", NULL, NULL,&m_pTextureRV, NULL);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "D3DX10CreateShaderResourceViewFromFile failed! hr:"<<hr;
		return false;
	}
	m_pDiffuseVariable = m_pEffect->GetVariableByName("txDiffuse")->AsShaderResource();
	return true;
}

void CDirectXWindow::SaveToDisk()
{
	ID3D10Texture2D *pTargetRenderTexture = NULL;
	MyCopyResource(&pTargetRenderTexture, m_TargetTexture->m_pTexture, m_pd3dDevice, L"C:\\testbmp\\GetScreen\\");
	SAFE_RELEASE(pTargetRenderTexture);
}

void CDirectXWindow::Transform()
{
	D3DXMatrixIdentity(&m_World);
	D3DXVECTOR3 Eye(0.0f, 0.0f, -5.0f);
	D3DXVECTOR3 At(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&m_View, &Eye, &At, &Up);

	//D3DXMatrixOrthoLH(&m_Projection, m_ptSize.x, m_ptSize.y, -100.0f, 100.0f);
	//D3DXMatrixPerspectiveLH(,0, , );

	D3DXMatrixOrthoLH(&m_Projection, m_ptSize.x, m_ptSize.y, 0.1f, 100.0f);
	//D3DXMatrixPerspectiveFovLH(&m_Projection, ((float)D3DX_PI * 0.25), m_ptSize.x/(m_ptSize.y*1.0f), 0.0f, 100.0f);

	//XMMatrixPerspectiveFovLH()
	D3DXMATRIX mTranslate;
	D3DXMATRIX mOrbit;
	D3DXMATRIX mSpin;
	D3DXMATRIX mScale;

	static float t = 0.0f;
	m_pWorldVariable->SetMatrix((float*)&m_World);
	m_pViewVariable->SetMatrix((float*)&m_View);
	m_pProjectionVariable->SetMatrix((float*)&m_Projection);
}

void CDirectXWindow::DrawTexture(bool bTexFrameResource)
{
	Transform();
	if (bTexFrameResource)
	{
		m_pDiffuseVariable->SetResource(m_pFrameResource);
	}
	else
	{
		m_pDiffuseVariable->SetResource(m_pTextureRV);
	}

	float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
	m_pd3dDevice->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
	m_pd3dDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
	// Render a triangle
	D3D10_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pTechnique->GetPassByIndex(p)->Apply(0);
		m_pd3dDevice->Draw(6, 0);
	}
}

void CDirectXWindow::Render()
{
	if (m_pd3dDevice && m_pSwapChain)
	{
		Transform();
		m_pDiffuseVariable->SetResource(m_pTextureRV);

		float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f }; // RGBA
		m_pd3dDevice->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
		m_pd3dDevice->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
		// Render a triangle
		D3D10_TECHNIQUE_DESC techDesc;
		m_pTechnique->GetDesc(&techDesc);
		for (UINT p = 0; p < techDesc.Passes; ++p)
		{
			m_pTechnique->GetPassByIndex(p)->Apply(0);
			m_pd3dDevice->Draw(6, 0);
		}
		HRESULT hr = m_pSwapChain->Present(0, 0);
		if (!SUCCEEDED(hr))
		{
			LOG(ERROR) << "m_pSwapChain->Present";
			return ;
		}
	}
}

void CDirectXWindow::SetTexRenderTarget(bool bStencil)
{
	if (bStencil)
	{
		m_pd3dDevice->OMSetRenderTargets(1, &m_TargetTexture->m_pRenderTarget, m_pDepthStencilView);
	}
	else
	{
		m_pd3dDevice->OMSetRenderTargets(1, &m_TargetTexture->m_pRenderTarget, NULL);
	}
}

void CDirectXWindow::SetWindowRenderTarget(bool bStencil)
{
	if (bStencil)
	{
		m_pd3dDevice->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	}
	else
	{
		m_pd3dDevice->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
	}
}

bool CDirectXWindow::Init(HWND hwnd, POINT &ptTargetSize,
							RECT& rcClipRect, IDXGIAdapter* pAdapter)
{
	if (m_hwnd)
	{
		LOG(ERROR) << "CDirectXWindow::Init already call!";
		return true;
	}

	if (!hwnd)
	{
		LOG(ERROR) << "CDirectXWindow::Init hwnd is invalid!";
		return false;
	}
	m_hwnd = hwnd;
	m_rcClipRect = rcClipRect;
	m_ptSize = ptTargetSize;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = m_ptSize.x;
	sd.BufferDesc.Height = m_ptSize.y;
	sd.BufferDesc.Format = /*DXGI_FORMAT_R8G8B8A8_UNORM*/DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	DXGI_ADAPTER_DESC desc;
	pAdapter->GetDesc(&desc);
	
	//UINT createFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
	UINT createFlags = D3D10_CREATE_DEVICE_DEBUG;
	D3D10_FEATURE_LEVEL1 level = D3D10_FEATURE_LEVEL_10_1;
	HRESULT hr = D3D10CreateDeviceAndSwapChain1(pAdapter, 
		D3D10_DRIVER_TYPE_HARDWARE, NULL, createFlags, level,
						D3D10_1_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "D3D10CreateDeviceAndSwapChain failed ! :"<< hr;
		return false;
	}
	
	// Create a render target view
	ID3D10Texture2D *pBackBuffer = NULL;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&pBackBuffer);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "m_pSwapChain->GetBuffer failed ! :"<< hr;
		return false;
	}
	// get the back buffer description
	D3D10_TEXTURE2D_DESC bufferdesc;
	pBackBuffer->GetDesc(&bufferdesc);
	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);

	SAFE_RELEASE(pBackBuffer);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "CreateRenderTargetView failed ! :"<< hr;
		return false;
	}
	
	D3D10_VIEWPORT vp;
	vp.Width = m_ptSize.x;
	vp.Height = m_ptSize.y;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_pd3dDevice->RSSetViewports(1, &vp);
	return true;
}

bool CDirectXWindow::InitModel()
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif
	ID3D10Blob* pErr = NULL;
	hr = D3DX10CreateEffectFromMemory(s_strEffect, sizeof(s_strEffect), NULL, NULL, NULL, 
										"fx_4_0", dwShaderFlags, 0, m_pd3dDevice, NULL, NULL, 
											&m_pEffect, &pErr, NULL);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << L"The FX file cannot be located.  Please run this executable from the directory that contains the FX file. HRESULT:"<< hr;
		LPCSTR wzInfo = (LPCSTR)pErr->GetBufferPointer();
		return hr;
	}

	m_pTechnique = m_pEffect->GetTechniqueByName("Render");

	// Define the input layout
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = sizeof(layout) / sizeof(layout[0]);

	// Create the input layout
	D3D10_PASS_DESC PassDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);
	hr = m_pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
											PassDesc.IAInputSignatureSize, &m_pVertexLayout);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "m_pd3dDevice->CreateInputLayout failed :" << hr;
		return false;
	}
	// Set the input layout
	m_pd3dDevice->IASetInputLayout(m_pVertexLayout);

	// Create vertex buffer
	SimpleVertex vertices[] =
	{
		//{ D3DXVECTOR3(1.0f, 1.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f) },
		//{ D3DXVECTOR3(1.0f, -1.0f, 0.5f), D3DXVECTOR2(1.0f, -1.0f) },
		//{ D3DXVECTOR3(-1.0f, -1.0f, 0.5f), D3DXVECTOR2(0.0f, -1.0f) },
		//{ D3DXVECTOR3(1.0f, 1.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f) },
		//{ D3DXVECTOR3(-1.0f, -1.0f, 0.5f), D3DXVECTOR2(0.0f, -1.0f) },
		//{ D3DXVECTOR3(-1.0f, 1.0f, 0.5f), D3DXVECTOR2(0.0f, 0.0f) },

		{ D3DXVECTOR3(m_ptSize.x / 2.0f, m_ptSize.y / 2.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(m_ptSize.x / 2.0f, m_ptSize.y / -2.0f, 0.5f), D3DXVECTOR2(1.0f, -1.0f) },
		{ D3DXVECTOR3(m_ptSize.x / -2.0f, m_ptSize.y / -2.0f, 0.5f), D3DXVECTOR2(0.0f, -1.0f) },
		{ D3DXVECTOR3(m_ptSize.x / 2.0f, m_ptSize.y / 2.0f, 0.5f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(m_ptSize.x / -2.0f, m_ptSize.y / -2.0f, 0.5f), D3DXVECTOR2(0.0f, -1.0f) },
		{ D3DXVECTOR3(m_ptSize.x / -2.0f, m_ptSize.y / 2.0f, 0.5f), D3DXVECTOR2(0.0f, 0.0f) },
	};
	D3D10_BUFFER_DESC bd;
	bd.Usage = D3D10_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex)* 6;
	bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = vertices;
	hr = m_pd3dDevice->CreateBuffer(&bd, &InitData, &m_pVertexBuffer);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "m_pd3dDevice->CreateBuffer failed! hr:"<<hr;
		return false;
	}

	// Set vertex buffer
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	m_pd3dDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// Set primitive topology
	m_pd3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pWorldVariable = m_pEffect->GetVariableByName("World")->AsMatrix();
	m_pViewVariable = m_pEffect->GetVariableByName("View")->AsMatrix();
	m_pProjectionVariable = m_pEffect->GetVariableByName("Projection")->AsMatrix();

	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_ptSize.x;
	descDepth.Height =m_ptSize.y;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencil);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "CreateTexture2D create depth stencil failed! hr:"<<hr;
		return false;
	}
	
	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
	if (!SUCCEEDED(hr))
	{
		LOG(ERROR) << "CreateDepthStencilView Failed! hr:" << hr;
		return hr;
	}
	//MapTextrue();
	return true;
}

// 
// DXGI_FORMAT DXGI_FORMAT_B8G8R8A8_UNORM, GS_BGRA
// 
bool CDirectXWindow::CreateTargetViewWithTex(DXGI_FORMAT dxgiFormat)
{
	HRESULT err = S_OK;
	DXGI_FORMAT format = dxgiFormat;

	D3D10_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = m_ptSize.x;
	td.Height = m_ptSize.y;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM/*format*/;
	td.BindFlags = /*D3D10_BIND_SHADER_RESOURCE | */D3D10_BIND_RENDER_TARGET;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D10_USAGE_DEFAULT;

	ID3D10Texture2D *texVal = NULL;
	err = m_pd3dDevice->CreateTexture2D(&td, NULL, &texVal);
	if (!SUCCEEDED(err))
	{
		LOG(ERROR) << "D3D10Texture::CreateRenderTarget: CreateTexture2D failed, result:" << err;
		return false;
	}

	//------------------------------------------
	//D3D10_SHADER_RESOURCE_VIEW_DESC resourceDesc;
	//ZeroMemory(&resourceDesc, sizeof(resourceDesc));
	//resourceDesc.Format = format;
	//resourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	//resourceDesc.Texture2D.MipLevels = 1;
	//ID3D10ShaderResourceView *resource = NULL;
	//err = m_pd3dDevice->CreateShaderResourceView(texVal, &resourceDesc, &resource);
	//if (!SUCCEEDED(err))
	//{
	//	SAFE_RELEASE(texVal);
	//	LOG(ERROR) << "D3D10Texture::CreateRenderTarget: CreateShaderResourceView failed, result" << err;
	//	return false;
	//}

	//------------------------------------------

	ID3D10RenderTargetView *view;
	err = m_pd3dDevice->CreateRenderTargetView(texVal, NULL, &view);
	if (FAILED(err))
	{
		SAFE_RELEASE(texVal);
		//SAFE_RELEASE(resource);
		LOG(ERROR) << "D3D10Texture::CreateRenderTarget: CreateRenderTargetView failed, result" << err;
		return NULL;
	}

	//------------------------------------------
	m_TargetTexture = new D3D10Texture;
	m_TargetTexture->m_format = format;
	//m_TargetTexture->m_pResource = resource;
	m_TargetTexture->m_pTexture = texVal;
	m_TargetTexture->m_pRenderTarget = view;
	m_TargetTexture->m_width = m_ptSize.x;
	m_TargetTexture->m_height = m_ptSize.y;

	return true;
}