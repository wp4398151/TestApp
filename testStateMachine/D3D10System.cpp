/********************************************************************************
 Copyright (C) 2012 Hugh Bailey <obs.jim@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/


#include "Main.h"

//extern "C" _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000000;

bool GetOutputAttachVideoCard(DWORD &deviceId)
{
	HRESULT err = S_OK;
	bool bRet = false;
#ifdef USE_DXGI1_2
	REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
	REFIIF iidVal = __uuidof(IDXGIFactory1);
#endif

	IDXGIFactory1 *factory = NULL;
	if (SUCCEEDED(err = CreateDXGIFactory1(iidVal, (void**)&factory)))
	{
		UINT i = 0;
		IDXGIAdapter1 *giAdapter = NULL;

		while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
		{
			//Log(TEXT("------------------------------------------"));
			DXGI_ADAPTER_DESC adapterDesc;
			if (SUCCEEDED(err = giAdapter->GetDesc(&adapterDesc)))
			{
				if (adapterDesc.DedicatedVideoMemory != 0 ||
					adapterDesc.SubSysId != 0)
				{
					UINT j = 0;
					IDXGIOutput *giOutput = NULL;
					while (giAdapter->EnumOutputs(j++, &giOutput) == S_OK)
					{
						DXGI_OUTPUT_DESC outputDesc;
						if (SUCCEEDED(giOutput->GetDesc(&outputDesc)))
						{
							if (outputDesc.AttachedToDesktop)
							{
								deviceId = adapterDesc.DeviceId;
								bRet = true;
							}
							else
							{
							}
						}
						giOutput->Release();
					}
				}
			}
			else
				AppWarning(TEXT("Could not query adapter %u"), i);

			giAdapter->Release();
		}
		factory->Release();
	}

	return bRet;
}

void GetDisplayDevices(DeviceOutputs &deviceList)
{
    HRESULT err = S_OK;

    deviceList.ClearData();

#ifdef USE_DXGI1_2
    REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
    REFIIF iidVal = __uuidof(IDXGIFactory1);
#endif

    IDXGIFactory1 *factory;
    if (SUCCEEDED(err = CreateDXGIFactory1(iidVal, (void**)&factory)))
    {
        UINT i=0;
        IDXGIAdapter1 *giAdapter;

        while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
        {
            //Log(TEXT("------------------------------------------"));

            DXGI_ADAPTER_DESC adapterDesc;
            if (SUCCEEDED(err = giAdapter->GetDesc(&adapterDesc)))
            {
                if (adapterDesc.DedicatedVideoMemory != 0 ||
                        adapterDesc.SubSysId != 0)
                {
                    DeviceOutputData &deviceData = *deviceList.devices.CreateNew();
                    deviceData.strDevice = adapterDesc.Description;

                    UINT j=0;
                    IDXGIOutput *giOutput;
                    while (giAdapter->EnumOutputs(j++, &giOutput) == S_OK)
                    {
                        DXGI_OUTPUT_DESC outputDesc;
                        if (SUCCEEDED(giOutput->GetDesc(&outputDesc)))
                        {
                            if (outputDesc.AttachedToDesktop)
                            {
                                deviceData.monitorNameList << outputDesc.DeviceName;

                                MonitorInfo &monitorInfo = *deviceData.monitors.CreateNew();
                                monitorInfo.hMonitor = outputDesc.Monitor;
                                mcpy(&monitorInfo.rect, &outputDesc.DesktopCoordinates, sizeof(RECT));
                                switch (outputDesc.Rotation)
                                {
                                case DXGI_MODE_ROTATION_ROTATE90:
                                    monitorInfo.rotationDegrees = 90.0f;
                                    break;
                                case DXGI_MODE_ROTATION_ROTATE180:
                                    monitorInfo.rotationDegrees = 180.0f;
                                    break;
                                case DXGI_MODE_ROTATION_ROTATE270:
                                    monitorInfo.rotationDegrees = 270.0f;
                                }
                            }
                        }

                        giOutput->Release();
                    }
                }
            }
            else
                AppWarning(TEXT("Could not query adapter %u"), i);

            giAdapter->Release();
        }

        factory->Release();
    }
}


void LogVideoCardStats()
{
    HRESULT err;

#ifdef USE_DXGI1_2
    REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
    REFIIF iidVal = __uuidof(IDXGIFactory1);
#endif

    IDXGIFactory1 *factory;
    if (SUCCEEDED(err = CreateDXGIFactory1(iidVal, (void**)&factory)))
    {
        UINT i=0;
        IDXGIAdapter1 *giAdapter;

        while (factory->EnumAdapters1(i++, &giAdapter) == S_OK)
        {
            DXGI_ADAPTER_DESC adapterDesc;
            if (SUCCEEDED(err = giAdapter->GetDesc(&adapterDesc)))
            {
                if (!(adapterDesc.VendorId == 0x1414 && adapterDesc.DeviceId == 0x8c))   // Ignore Microsoft Basic Render Driver
                {
                    Log(TEXT("------------------------------------------"));
                    Log(TEXT("Adapter %u"), i);
                    Log(TEXT("  Video Adapter: %s"), adapterDesc.Description);
                    Log(TEXT("  Video Adapter Dedicated Video Memory: %u"), adapterDesc.DedicatedVideoMemory);
                    Log(TEXT("  Video Adapter Shared System Memory: %u"), adapterDesc.SharedSystemMemory);

                    UINT j = 0;
                    IDXGIOutput *output;
                    while (SUCCEEDED(giAdapter->EnumOutputs(j++, &output)))
                    {
                        DXGI_OUTPUT_DESC desc;
                        if (SUCCEEDED(output->GetDesc(&desc)))
                            Log(TEXT("  Video Adapter Output %u: pos={%d, %d}, size={%d, %d}, attached=%s"), j,
                                desc.DesktopCoordinates.left, desc.DesktopCoordinates.top,
                                desc.DesktopCoordinates.right-desc.DesktopCoordinates.left, desc.DesktopCoordinates.bottom-desc.DesktopCoordinates.top,
                                desc.AttachedToDesktop ? L"true" : L"false");
                        output->Release();
                    }
                }
            }
            else
                AppWarning(TEXT("Could not query adapter %u"), i);

            giAdapter->Release();
        }

        factory->Release();
    }
}

static void HandleNvidiaOptimus(IDXGIFactory1 *factory, IDXGIAdapter1 *&adapter, UINT &adapterID)
{
    if (adapterID != 1)
        return;

    //NvOptimusEnablement = 0;
    DXGI_ADAPTER_DESC adapterDesc;
    if (SUCCEEDED(adapter->GetDesc(&adapterDesc)))
    {
        String name = adapterDesc.Description;
        name.KillSpaces();

        if (name.IsEmpty())
            return;

        if (sstri(adapterDesc.Description, L"NVIDIA") != NULL)
        {
            if (name[name.Length()-1] == 'M' || name[name.Length()-1] == 'm')
            {
                adapter->Release();

                adapterID = 0;
                //NvOptimusEnablement = 1;
                Log(L"Nvidia optimus detected, second adapter selected, ignoring useless second adapter, I guess.");
                if (FAILED(factory->EnumAdapters1(adapterID, &adapter)))
                    CrashError(TEXT("Could not get DXGI adapter"));
            }
        }
    }
}

D3D10System::D3D10System()
{
    HRESULT err;
#ifdef USE_DXGI1_2
    REFIID iidVal = OSGetVersion() >= 8 ? __uuidof(IDXGIFactory2) : __uuidof(IDXGIFactory1);
#else
    REFIID iidVal = __uuidof(IDXGIFactory1);
#endif
    UINT adapterID = 0;	// adapter index

	if (FAILED(err = CreateDXGIFactory1(iidVal, (void**)&m_pFactory)))
		CrashError(TEXT("Could not create DXGI factory"));
	UINT i = 0;
	if (false == GetOutputAttachVideoCard(g_pOBSApp->m_DeviceID))
	{
		CrashError(TEXT("GetOutputAttachVideoCard"));
	}

	UINT adapterDeviceID = g_pOBSApp->m_DeviceID;		// if device id equal zero, use the first device

	DXGI_ADAPTER_DESC dxgiDesc;

    IDXGIAdapter1 *giAdapter = NULL;
    if (adapterDeviceID != 0 )
	{
		while (m_pFactory->EnumAdapters1(i, &giAdapter) == S_OK)
		{
			ZeroMemory(&dxgiDesc, sizeof(dxgiDesc));
			giAdapter->GetDesc(&dxgiDesc);
			if (adapterDeviceID == dxgiDesc.DeviceId)
			{
				adapterID = i;
				break;
			}
			++i;
			giAdapter->Release();
		}
	}

	giAdapter->Release();
	// 选择显卡的方式需要使用DeviceID， 而不仅仅是 deviceIndex, 否则会因为版本不一样，轮询的顺序会发生变化。
	// 可选择的DeviceID不再使用。每次都是动态去选择

	UINT numAdapters = 0;
	i = 0;
    giAdapter = NULL;

    while (m_pFactory->EnumAdapters1(i++, &giAdapter) == S_OK)
    {
        // 通过判断显卡中的描述来判断当前的显卡类型，是不是NV的
        ZeroMemory(&dxgiDesc, sizeof(dxgiDesc));
        giAdapter->GetDesc(&dxgiDesc);
        if (sstri(dxgiDesc.Description, L"NVIDIA") != NULL)
        {
            g_pOBSApp->m_fAdapterType |= ADAPTA_NVIDA;
        }
		else if (sstri(dxgiDesc.Description, L"AMD") != NULL || sstri(dxgiDesc.Description, L"Radeon") != NULL)
        {
            g_pOBSApp->m_fAdapterType |= ADAPTA_AMD;
        }
        else if (sstri(dxgiDesc.Description, L"Intel") != NULL)
        {
            g_pOBSApp->m_fAdapterType |= ADAPTA_INTEL;
        }

        numAdapters++;
        giAdapter->Release();
    }

    if (g_pOBSApp->m_fAdapterType & ADAPTA_INTEL)
    {
        ++g_pOBSApp->m_cAdapterCount;
    }

    if (g_pOBSApp->m_fAdapterType & ADAPTA_AMD)
    {
        ++g_pOBSApp->m_cAdapterCount;
    }

    if (g_pOBSApp->m_fAdapterType & ADAPTA_NVIDA)
    {
        ++g_pOBSApp->m_cAdapterCount;
    }

    if (adapterID >= numAdapters)
    {
        Log(TEXT("Invalid adapter id %d, only %d adapters on system. Resetting to 0."), adapterID, numAdapters);
        g_pGlobalConfig->SetInt(TEXT("Video"), TEXT("Adapter"), 0);
        adapterID = 0;
    }

    IDXGIAdapter1 *adapter;
    if (FAILED(err = m_pFactory->EnumAdapters1(adapterID, &adapter)))
        CrashError(TEXT("Could not get DXGI adapter %d"), adapterID);

    //HandleNvidiaOptimus(m_pFactory, adapter, adapterID);

    //------------------------------------------------------------------

    DXGI_SWAP_CHAIN_DESC swapDesc;
    zero(&swapDesc, sizeof(swapDesc));
    swapDesc.BufferCount = 2;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDesc.BufferDesc.Width  = g_pOBSApp->m_nRenderFrameWidth;
    swapDesc.BufferDesc.Height = g_pOBSApp->m_nRenderFrameHeight;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.OutputWindow = g_hwndMain;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.Windowed = TRUE;

    m_bDisableCompatibilityMode = 1;//AppConfig->GetInt(TEXT("Video"), TEXT("DisableD3DCompatibilityMode"), 1) != 0;

    UINT createFlags = D3D10_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        createFlags |= D3D10_CREATE_DEVICE_DEBUG;
#else
	if (g_pGlobalConfig->GetInt(TEXT("General"), TEXT("UseDebugD3D")))
		createFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

    D3D10_FEATURE_LEVEL1 level = m_bDisableCompatibilityMode ? D3D10_FEATURE_LEVEL_10_1 : D3D10_FEATURE_LEVEL_9_3;

    String adapterName;
    DXGI_ADAPTER_DESC desc;
    if (adapter->GetDesc(&desc) == S_OK)
        adapterName = desc.Description;
    else
        adapterName = TEXT("<unknown>");

    adapterName.KillSpaces();

    Log(TEXT("Loading up D3D10 on %s (Adapter %u) DeviceId %d..."), adapterName.Array(), adapterID+1, desc.DeviceId);
    err = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, createFlags, level, D3D10_1_SDK_VERSION, &swapDesc, &m_pSwap, &m_pD3d);
    if (FAILED(err))
    {
        Log (TEXT("D3D10CreateDeviceAndSwapChain1: Failed on %s: 0x%08x. Trying compatibility mode"), adapterName.Array(), err);
        m_bDisableCompatibilityMode = !m_bDisableCompatibilityMode;
        level = m_bDisableCompatibilityMode ? D3D10_FEATURE_LEVEL_10_1 : D3D10_FEATURE_LEVEL_9_3;
        err = D3D10CreateDeviceAndSwapChain1(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, createFlags, level, D3D10_1_SDK_VERSION, &swapDesc, &m_pSwap, &m_pD3d);
    }

    if (FAILED(err))
    {
        Log (TEXT("D3D10CreateDeviceAndSwapChain1: Failed on %s: 0x%08x"), adapterName.Array(), err);
        CrashError(TEXT("Could not initialize DirectX 10 on %s.  This error can happen for one of the following reasons:\r\n\r\n1.) Your GPU is not supported (DirectX 10 is required - note that many integrated laptop GPUs do not support DX10)\r\n2.) You're running Windows Vista without the \"Platform Update\"\r\n3.) Your video card drivers are out of date\r\n\r\nIf you are using a laptop with NVIDIA Optimus or AMD Switchable Graphics, make sure GameRecorder is set to run on the high performance GPU in your driver settings."), adapterName.Array());
    }

    adapter->Release();

    //------------------------------------------------------------------

    D3D10_DEPTH_STENCIL_DESC depthDesc;
    zero(&depthDesc, sizeof(depthDesc));
    depthDesc.DepthEnable = FALSE;

    err = m_pD3d->CreateDepthStencilState(&depthDesc, &m_pDepthState);
    if (FAILED(err))
        CrashError(TEXT("Unable to create depth state"));

    m_pD3d->OMSetDepthStencilState(m_pDepthState, 0);

    //------------------------------------------------------------------

    D3D10_RASTERIZER_DESC rasterizerDesc;
    zero(&rasterizerDesc, sizeof(rasterizerDesc));
    rasterizerDesc.FillMode = D3D10_FILL_SOLID;
    rasterizerDesc.CullMode = D3D10_CULL_NONE;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.DepthClipEnable = TRUE;

    err = m_pD3d->CreateRasterizerState(&rasterizerDesc, &m_pRasterizerState);
    if (FAILED(err))
        CrashError(TEXT("Unable to create rasterizer state"));

    m_pD3d->RSSetState(m_pRasterizerState);

    //------------------------------------------------------------------

    rasterizerDesc.ScissorEnable = TRUE;

    err = m_pD3d->CreateRasterizerState(&rasterizerDesc, &m_pScissorState);
    if (FAILED(err))
        CrashError(TEXT("Unable to create scissor state"));

    //------------------------------------------------------------------

    ID3D10Texture2D *backBuffer = NULL;
    err = m_pSwap->GetBuffer(0, IID_ID3D10Texture2D, (void**)&backBuffer);
    if (FAILED(err))
        CrashError(TEXT("Unable to get back buffer from swap chain"));

    err = m_pD3d->CreateRenderTargetView(backBuffer, NULL, &m_pSwapRenderView);
    if (FAILED(err))
        CrashError(TEXT("Unable to get render view from back buffer"));

    backBuffer->Release();

    //------------------------------------------------------------------

    D3D10_BLEND_DESC disabledBlendDesc;
    zero(&disabledBlendDesc, sizeof(disabledBlendDesc));
    for (int ii=0; ii<8; ii++)
    {
        disabledBlendDesc.BlendEnable[ii]        = TRUE;
        disabledBlendDesc.RenderTargetWriteMask[ii] = D3D10_COLOR_WRITE_ENABLE_ALL;
    }
    disabledBlendDesc.BlendOpAlpha          = D3D10_BLEND_OP_ADD;
    disabledBlendDesc.BlendOp               = D3D10_BLEND_OP_ADD;
    disabledBlendDesc.SrcBlendAlpha         = D3D10_BLEND_ONE;
    disabledBlendDesc.DestBlendAlpha        = D3D10_BLEND_ZERO;
    disabledBlendDesc.SrcBlend              = D3D10_BLEND_ONE;
    disabledBlendDesc.DestBlend             = D3D10_BLEND_ZERO;

    err = m_pD3d->CreateBlendState(&disabledBlendDesc, &m_pDisabledBlend);
    if (FAILED(err))
        CrashError(TEXT("Unable to create disabled blend state"));

    this->BlendFunction(GS_BLEND_SRCALPHA, GS_BLEND_INVSRCALPHA, 1.0f);
    m_bBlendingEnabled = true;
}

D3D10System::~D3D10System()
{
    delete m_pSpriteVertexBuffer;
    delete m_pBoxVertexBuffer;

    for (UINT i=0; i<m_blends.Num(); i++)
        SafeRelease(m_blends[i].blendState);

    SafeRelease(m_pScissorState);
    SafeRelease(m_pRasterizerState);
    SafeRelease(m_pDepthState);
    SafeRelease(m_pDisabledBlend);
    SafeRelease(m_pSwapRenderView);
    SafeRelease(m_pSwap);
    SafeRelease(m_pD3d);
    SafeRelease(m_pFactory);
}

void D3D10System::UnloadAllData()
{
    LoadVertexShader(NULL);
    LoadPixelShader(NULL);
    LoadVertexBuffer(NULL);
    for (UINT i=0; i<8; i++)
    {
        LoadSamplerState(NULL, i);
        LoadTexture(NULL, i);
    }

    UINT zeroVal = 0;
    LPVOID nullBuff[8];
    float bla[4] = {1.0f, 1.0f, 1.0f, 1.0f};

    zero(nullBuff, sizeof(nullBuff));

    m_pD3d->VSSetConstantBuffers(0, 1, (ID3D10Buffer**)nullBuff);
    m_pD3d->PSSetConstantBuffers(0, 1, (ID3D10Buffer**)nullBuff);
    m_pD3d->OMSetDepthStencilState(NULL, 0);
    m_pD3d->PSSetSamplers(0, 1, (ID3D10SamplerState**)nullBuff);
    m_pD3d->OMSetBlendState(NULL, bla, 0xFFFFFFFF);
    m_pD3d->OMSetRenderTargets(1, (ID3D10RenderTargetView**)nullBuff, NULL);
    m_pD3d->IASetVertexBuffers(0, 8, (ID3D10Buffer**)nullBuff, &zeroVal, &zeroVal);
    m_pD3d->PSSetShaderResources(0, 8, (ID3D10ShaderResourceView**)nullBuff);
    m_pD3d->IASetInputLayout(NULL);
    m_pD3d->PSSetShader(NULL);
    m_pD3d->VSSetShader(NULL);
    m_pD3d->RSSetState(NULL);
    m_pD3d->RSSetScissorRects(0, NULL);
}

LPVOID D3D10System::GetDevice()
{
    return (LPVOID)m_pD3d;
}


void D3D10System::Init()
{
	VBData *data = new VBData;
	data->UVList.SetSize(1);

	data->VertList.SetSize(4);
	data->UVList[0].SetSize(4);

	m_pSpriteVertexBuffer = CreateVertexBuffer(data, FALSE);

	//------------------------------------------------------------------
	VBData *data1 = NULL;
	data1 = new VBData;
	data1->VertList.SetSize(5);
	m_pBoxVertexBuffer = CreateVertexBuffer(data1, FALSE);

	//------------------------------------------------------------------

	GraphicsSystem::Init();
}


////////////////////////////
//Texture Functions
Texture* D3D10System::CreateTextureFromSharedHandle(unsigned int width, unsigned int height, HANDLE handle)
{
    return D3D10Texture::CreateFromSharedHandle(width, height, handle);
}

Texture* D3D10System::CreateSharedTexture(unsigned int width, unsigned int height)
{
    return D3D10Texture::CreateShared(width, height);
}

Texture* D3D10System::CreateTexture(unsigned int width, unsigned int height, GSColorFormat colorFormat, void *lpData, BOOL bBuildMipMaps, BOOL bStatic)
{
    return D3D10Texture::CreateTexture(width, height, colorFormat, lpData, bBuildMipMaps, bStatic);
}

Texture* D3D10System::CreateTextureFromFile(CTSTR lpFile, BOOL bBuildMipMaps)
{
    return D3D10Texture::CreateFromFile(lpFile, bBuildMipMaps);
}

Texture* D3D10System::CreateRenderTarget(unsigned int width, unsigned int height, GSColorFormat colorFormat, BOOL bGenMipMaps)
{
    return D3D10Texture::CreateRenderTarget(width, height, colorFormat, bGenMipMaps);
}

Texture* D3D10System::CreateGDITexture(unsigned int width, unsigned int height)
{
    return D3D10Texture::CreateGDITexture(width, height);
}

bool D3D10System::GetTextureFileInfo(CTSTR lpFile, TextureInfo &info)
{
    D3DX10_IMAGE_INFO ii;
    if (SUCCEEDED(D3DX10GetImageInfoFromFile(lpFile, NULL, &ii, NULL)))
    {
        info.width = ii.Width;
        info.height = ii.Height;
        switch (ii.Format)
        {
        case DXGI_FORMAT_A8_UNORM:
            info.type = GS_ALPHA;
            break;
        case DXGI_FORMAT_R8_UNORM:
            info.type = GS_GRAYSCALE;
            break;
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            info.type = GS_BGR;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            info.type = GS_BGRA;
            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            info.type = GS_RGBA;
            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            info.type = GS_RGBA16F;
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            info.type = GS_RGBA32F;
            break;
        case DXGI_FORMAT_BC1_UNORM:
            info.type = GS_DXT1;
            break;
        case DXGI_FORMAT_BC2_UNORM:
            info.type = GS_DXT3;
            break;
        case DXGI_FORMAT_BC3_UNORM:
            info.type = GS_DXT5;
            break;
        default:
            info.type = GS_UNKNOWNFORMAT;
        }

        return true;
    }

    return false;
}

SamplerState* D3D10System::CreateSamplerState(SamplerInfo &info)
{
    return D3D10SamplerState::CreateSamplerState(info);
}


UINT D3D10System::GetNumOutputs()
{
    UINT count = 0;

    IDXGIDevice *device;
    if (SUCCEEDED(m_pD3d->QueryInterface(__uuidof(IDXGIDevice), (void**)&device)))
    {
        IDXGIAdapter *adapter;
        if (SUCCEEDED(device->GetAdapter(&adapter)))
        {
            IDXGIOutput *outputInterface;

            while (SUCCEEDED(adapter->EnumOutputs(count, &outputInterface)))
            {
                count++;
                outputInterface->Release();
            }

            adapter->Release();
        }

        device->Release();
    }

    return count;
}

OutputDuplicator *D3D10System::CreateOutputDuplicator(UINT outputID)
{
    D3D10OutputDuplicator *duplicator = new D3D10OutputDuplicator;
    if (duplicator->Init(outputID))
        return duplicator;

    delete duplicator;
    return NULL;
}


////////////////////////////
//Shader Functions
Shader* D3D10System::CreateVertexShaderFromBlob(ShaderBlob const &blob, CTSTR lpShader, CTSTR lpFileName)
{
    return D3D10VertexShader::CreateVertexShaderFromBlob(blob, lpShader, lpFileName);
}

Shader* D3D10System::CreatePixelShaderFromBlob(ShaderBlob const &blob, CTSTR lpShader, CTSTR lpFileName)
{
    return D3D10PixelShader::CreatePixelShaderFromBlob(blob, lpShader, lpFileName);
}

Shader* D3D10System::CreateVertexShader(CTSTR lpShader, CTSTR lpFileName)
{
    return D3D10VertexShader::CreateVertexShader(lpShader, lpFileName);
}

Shader* D3D10System::CreatePixelShader(CTSTR lpShader, CTSTR lpFileName)
{
    return D3D10PixelShader::CreatePixelShader(lpShader, lpFileName);
}

void D3D10System::CreateVertexShaderBlob(ShaderBlob &blob, CTSTR lpShader, CTSTR lpFileName)
{
    D3D10VertexShader::CreateVertexShaderBlob(blob, lpShader, lpFileName);
}

void D3D10System::CreatePixelShaderBlob(ShaderBlob &blob, CTSTR lpShader, CTSTR lpFileName)
{
    D3D10PixelShader::CreatePixelShaderBlob(blob, lpShader, lpFileName);
}


////////////////////////////
//Vertex Buffer Functions
VertexBuffer* D3D10System::CreateVertexBuffer(VBData *vbData, BOOL bStatic)
{
    return D3D10VertexBuffer::CreateVertexBuffer(vbData, bStatic);
}


////////////////////////////
//Main Rendering Functions
void D3D10System::LoadVertexBuffer(VertexBuffer* vb)
{
    if (vb != m_pCurVertexBuffer)
    {
        D3D10VertexBuffer *d3dVB = static_cast<D3D10VertexBuffer*>(vb);
        if (m_pCurVertexShader)
        {
            List<ID3D10Buffer*> buffers;
            List<UINT> strides;
            List<UINT> offsets;

            if (d3dVB)
                d3dVB->MakeBufferList(m_pCurVertexShader, buffers, strides);
            else
            {
                UINT nBuffersToClear = m_pCurVertexShader->NumBuffersExpected();
                buffers.SetSize(nBuffersToClear);
                strides.SetSize(nBuffersToClear);
            }

            offsets.SetSize(buffers.Num());
            m_pD3d->IASetVertexBuffers(0, buffers.Num(), buffers.Array(), strides.Array(), offsets.Array());
        }

        m_pCurVertexBuffer = d3dVB;
    }
}

void D3D10System::LoadTexture(Texture *texture, UINT idTexture)
{
    if (m_pCurTextures[idTexture] != texture)
    {
        D3D10Texture *d3dTex = static_cast<D3D10Texture*>(texture);
        if (d3dTex)
            m_pD3d->PSSetShaderResources(idTexture, 1, &d3dTex->m_pResource);
        else
        {
            LPVOID lpNull = NULL;
            m_pD3d->PSSetShaderResources(idTexture, 1, (ID3D10ShaderResourceView**)&lpNull);
        }

        m_pCurTextures[idTexture] = d3dTex;
    }
}

void D3D10System::LoadSamplerState(SamplerState *sampler, UINT idSampler)
{
    if (m_pCurSamplers[idSampler] != sampler)
    {
        D3D10SamplerState *d3dSampler = static_cast<D3D10SamplerState*>(sampler);
        if (d3dSampler)
            m_pD3d->PSSetSamplers(idSampler, 1, &d3dSampler->m_pState);
        else
        {
            LPVOID lpNull = NULL;
            m_pD3d->PSSetSamplers(idSampler, 1, (ID3D10SamplerState**)&lpNull);
        }
        m_pCurSamplers[idSampler] = d3dSampler;
    }
}

void D3D10System::LoadVertexShader(Shader *vShader)
{
    if (m_pCurVertexShader != vShader)
    {
        if (vShader)
        {
            D3D10VertexBuffer *lastVertexBuffer = m_pCurVertexBuffer;
            if (m_pCurVertexBuffer)
                LoadVertexBuffer(NULL);

            D3D10VertexShader *shader = static_cast<D3D10VertexShader*>(vShader);

            m_pD3d->VSSetShader(shader->m_pVertexShader);
            m_pD3d->IASetInputLayout(shader->m_pInputLayout);
            m_pD3d->VSSetConstantBuffers(0, 1, &shader->m_pConstantBuffer);

            if (lastVertexBuffer)
                LoadVertexBuffer(lastVertexBuffer);
        }
        else
        {
            LPVOID lpNULL = NULL;

            m_pD3d->VSSetShader(NULL);
            m_pD3d->VSSetConstantBuffers(0, 1, (ID3D10Buffer**)&lpNULL);
        }

        m_pCurVertexShader = static_cast<D3D10VertexShader*>(vShader);
    }
}

void D3D10System::LoadPixelShader(Shader *pShader)
{
    if (m_pCurPixelShader != pShader)
    {
        if (pShader)
        {
            D3D10PixelShader *shader = static_cast<D3D10PixelShader*>(pShader);

            m_pD3d->PSSetShader(shader->m_pPixelShader);
            m_pD3d->PSSetConstantBuffers(0, 1, &shader->m_pConstantBuffer);

            for (UINT i=0; i<shader->m_Samplers.Num(); i++)
                LoadSamplerState(shader->m_Samplers[i].sampler, i);
        }
        else
        {
            LPVOID lpNULL = NULL;

            m_pD3d->PSSetShader(NULL);
            m_pD3d->PSSetConstantBuffers(0, 1, (ID3D10Buffer**)&lpNULL);

            for (UINT i=0; i<8; i++)
                m_pCurSamplers[i] = NULL;

            ID3D10SamplerState *states[8];
            zero(states, sizeof(states));
            m_pD3d->PSSetSamplers(0, 8, states);
        }

        m_pCurPixelShader = static_cast<D3D10PixelShader*>(pShader);
    }
}

Shader* D3D10System::GetCurrentPixelShader()
{
    return m_pCurPixelShader;
}

Shader* D3D10System::GetCurrentVertexShader()
{
    return m_pCurVertexShader;
}

void D3D10System::SetRenderTarget(Texture *texture)
{
    if (m_pCurRenderTarget != texture)
    {
        if (texture)
        {
            ID3D10RenderTargetView *view = static_cast<D3D10Texture*>(texture)->m_pRenderTarget;
            if (!view)
            {
                AppWarning(TEXT("tried to set a texture that wasn't a render target as a render target"));
                return;
            }

            m_pD3d->OMSetRenderTargets(1, &view, NULL);
        }
        else
            m_pD3d->OMSetRenderTargets(1, &m_pSwapRenderView, NULL);

        m_pCurRenderTarget = static_cast<D3D10Texture*>(texture);
    }
}

const D3D10_PRIMITIVE_TOPOLOGY topologies[] = {D3D10_PRIMITIVE_TOPOLOGY_POINTLIST, D3D10_PRIMITIVE_TOPOLOGY_LINELIST, D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP};

void D3D10System::Draw(GSDrawMode drawMode, DWORD startVert, DWORD nVerts)
{
    if (!m_pCurVertexBuffer)
    {
        AppWarning(TEXT("Tried to call draw without setting a vertex buffer"));
        return;
    }

    if (!m_pCurVertexShader)
    {
        AppWarning(TEXT("Tried to call draw without setting a vertex shader"));
        return;
    }

    if (!m_pCurPixelShader)
    {
        AppWarning(TEXT("Tried to call draw without setting a pixel shader"));
        return;
    }

    m_pCurVertexShader->SetMatrix(m_pCurVertexShader->GetViewProj(), m_curViewProjMatrix);

    m_pCurVertexShader->UpdateParams();
    m_pCurPixelShader->UpdateParams();

    D3D10_PRIMITIVE_TOPOLOGY newTopology = topologies[(int)drawMode];
    if (newTopology != m_curTopology)
    {
        m_pD3d->IASetPrimitiveTopology(newTopology);
        m_curTopology = newTopology;
    }

    if (nVerts == 0)
        nVerts = static_cast<D3D10VertexBuffer*>(m_pCurVertexBuffer)->m_numVerts;

    m_pD3d->Draw(nVerts, startVert);
}


////////////////////////////
//Drawing mode functions

const D3D10_BLEND blendConvert[] = {D3D10_BLEND_ZERO, D3D10_BLEND_ONE, D3D10_BLEND_SRC_COLOR, D3D10_BLEND_INV_SRC_COLOR, D3D10_BLEND_SRC_ALPHA, D3D10_BLEND_INV_SRC_ALPHA, D3D10_BLEND_DEST_COLOR, D3D10_BLEND_INV_DEST_COLOR, D3D10_BLEND_DEST_ALPHA, D3D10_BLEND_INV_DEST_ALPHA, D3D10_BLEND_BLEND_FACTOR, D3D10_BLEND_INV_BLEND_FACTOR};

void  D3D10System::EnableBlending(BOOL bEnable)
{
    if (m_bBlendingEnabled != bEnable)
    {
        if ( (m_bBlendingEnabled = bEnable) == TRUE )
            m_pD3d->OMSetBlendState(m_pCurBlendState, m_curBlendFactor, 0xFFFFFFFF);
        else
            m_pD3d->OMSetBlendState(m_pDisabledBlend, m_curBlendFactor, 0xFFFFFFFF);
    }
}

void D3D10System::BlendFunction(GSBlendType srcFactor, GSBlendType destFactor, float fFactor)
{
    bool bUseFactor = (srcFactor >= GS_BLEND_FACTOR || destFactor >= GS_BLEND_FACTOR);

    if (bUseFactor)
        m_curBlendFactor[0] = m_curBlendFactor[1] = m_curBlendFactor[2] = m_curBlendFactor[3] = fFactor;

    for (UINT i=0; i<m_blends.Num(); i++)
    {
        SavedBlendState &blendInfo = m_blends[i];
        if (blendInfo.srcFactor == srcFactor && blendInfo.destFactor == destFactor)
        {
            if (bUseFactor || m_pCurBlendState != blendInfo.blendState)
            {
                m_pD3d->OMSetBlendState(blendInfo.blendState, m_curBlendFactor, 0xFFFFFFFF);
                m_pCurBlendState = blendInfo.blendState;
            }
            return;
        }
    }

    //blend wasn't found, create a new one and save it for later
    D3D10_BLEND_DESC blendDesc;
    zero(&blendDesc, sizeof(blendDesc));
    for (int i=0; i<8; i++)
    {
        blendDesc.BlendEnable[i]            = TRUE;
        blendDesc.RenderTargetWriteMask[i]  = D3D10_COLOR_WRITE_ENABLE_ALL;
    }
    blendDesc.BlendOpAlpha              = D3D10_BLEND_OP_ADD;
    blendDesc.BlendOp                   = D3D10_BLEND_OP_ADD;
    blendDesc.SrcBlendAlpha             = D3D10_BLEND_ONE;
    blendDesc.DestBlendAlpha            = D3D10_BLEND_ZERO;
    blendDesc.SrcBlend                  = blendConvert[srcFactor];
    blendDesc.DestBlend                 = blendConvert[destFactor];

    SavedBlendState *savedBlend = m_blends.CreateNew();
    savedBlend->destFactor      = destFactor;
    savedBlend->srcFactor       = srcFactor;

    if (FAILED(m_pD3d->CreateBlendState(&blendDesc, &savedBlend->blendState)))
        CrashError(TEXT("Could not set blend state"));

    if (m_bBlendingEnabled)
        m_pD3d->OMSetBlendState(savedBlend->blendState, m_curBlendFactor, 0xFFFFFFFF);

    m_pCurBlendState = savedBlend->blendState;
}

void D3D10System::ClearColorBuffer(DWORD color)
{
    Color4 floatColor;
    floatColor.MakeFromRGBA(color);

    D3D10Texture *d3dTex = static_cast<D3D10Texture*>(m_pCurRenderTarget);
    if (d3dTex)
        m_pD3d->ClearRenderTargetView(d3dTex->m_pRenderTarget, floatColor.ptr);
    else
        m_pD3d->ClearRenderTargetView(m_pSwapRenderView, floatColor.ptr);
}


////////////////////////////
//Other Functions
void D3D10System::Ortho(float left, float right, float top, float bottom, float znear, float zfar)
{
    Matrix4x4Ortho(m_curProjMatrix, left, right, top, bottom, znear, zfar);
    ResetViewMatrix();
}

void D3D10System::Frustum(float left, float right, float top, float bottom, float znear, float zfar)
{
    Matrix4x4Frustum(m_curProjMatrix, left, right, top, bottom, znear, zfar);
    ResetViewMatrix();
}


void D3D10System::SetViewport(float x, float y, float width, float height)
{
    D3D10_VIEWPORT vp;
    zero(&vp, sizeof(vp));
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = INT(x);
    vp.TopLeftY = INT(y);
    vp.Width    = UINT(width);
    vp.Height   = UINT(height);
    m_pD3d->RSSetViewports(1, &vp);
}

void D3D10System::SetScissorRect(XRect *pRect)
{
    if (pRect)
    {
        m_pD3d->RSSetState(m_pScissorState);
        D3D10_RECT rc = {pRect->x, pRect->y, pRect->x+pRect->cx, pRect->y+pRect->cy};
        m_pD3d->RSSetScissorRects(1, &rc);
    }
    else
    {
        m_pD3d->RSSetState(m_pRasterizerState);
        m_pD3d->RSSetScissorRects(0, NULL);
    }
}

//(jim) hey, I changed this to x, y, x2, y2
void D3D10System::SetCropping(float left, float top, float right, float bottom)
{
    m_curCropping[0] = left;
    m_curCropping[1] = top;
    m_curCropping[2] = right;
    m_curCropping[3] = bottom;
}

void D3D10System::DrawSpriteEx(Texture *texture, DWORD color, float x, float y, float x2, float y2, float u, float v, float u2, float v2)
{
    DrawSpriteExRotate(texture, color, x, y, x2, y2, 0.0f, u, v, u2, v2, 0.0f);
}

void D3D10System::DrawSpriteExRotate(Texture *texture, DWORD color, float x, float y, float x2, float y2, float degrees, float u, float v, float u2, float v2, float texDegrees)
{
    if (!m_pCurPixelShader)
        return;

    if (!texture)
    {
        AppWarning(TEXT("Trying to draw a sprite with a NULL texture"));
        return;
    }
    HANDLE hColor = m_pCurPixelShader->GetParameterByName(TEXT("outputColor"));

    if (hColor)
        m_pCurPixelShader->SetColor(hColor, color);

    //------------------------------
    // crop positional values

    Vect2 totalSize = Vect2(x2-x, y2-y);
    Vect2 invMult = Vect2(totalSize.P2.x < 0.0f ? -1.0f : 1.0f, totalSize.P2.y < 0.0f ? -1.0f : 1.0f);
    totalSize.Abs();

    if (y2-y < 0)
    {
        float tempFloat = m_curCropping[1];
        m_curCropping[1] = m_curCropping[3];
        m_curCropping[3] = tempFloat;
    }

    if (x2-x < 0)
    {
        float tempFloat = m_curCropping[0];
        m_curCropping[0] = m_curCropping[2];
        m_curCropping[2] = tempFloat;
    }

    bool bFlipX = (x2 - x) < 0.0f;
    bool bFlipY = (y2 - y) < 0.0f;

    x += m_curCropping[0] * invMult.P2.x;
    y += m_curCropping[1] * invMult.P2.y;
    x2 -= m_curCropping[2] * invMult.P2.x;
    y2 -= m_curCropping[3] * invMult.P2.y;

    bool cropXUnder = bFlipX ? ((x - x2) < 0.0f) : ((x2 - x) < 0.0f);
    bool cropYUnder = bFlipY ? ((y - y2) < 0.0f) : ((y2 - y) < 0.0f);

    // cropped out completely (eg mouse cursor texture)
    if (cropXUnder || cropYUnder)
        return;

    //------------------------------
    // crop texture coordinate values

    float cropMult[4];
    cropMult[0] = m_curCropping[0] / totalSize.P2.x;
    cropMult[1] = m_curCropping[1] / totalSize.P2.y;
    cropMult[2] = m_curCropping[2] / totalSize.P2.x;
    cropMult[3] = m_curCropping[3] / totalSize.P2.y;

    Vect2 totalUVSize = Vect2(u2-u, v2-v);
    u += cropMult[0] * totalUVSize.P2.x;
    v += cropMult[1] * totalUVSize.P2.y;
    u2 -= cropMult[2] * totalUVSize.P2.x;
    v2 -= cropMult[3] * totalUVSize.P2.y;

    //------------------------------
    // draw

    VBData *data = m_pSpriteVertexBuffer->GetData();
    data->VertList[0].Set(x,  y,  0.0f);
    data->VertList[1].Set(x,  y2, 0.0f);
    data->VertList[2].Set(x2, y,  0.0f);
    data->VertList[3].Set(x2, y2, 0.0f);

    if (!CloseFloat(degrees, 0.0f))
    {
        List<Vect> &coords = data->VertList;

        Vect2 center(x + totalSize.P2.x / 2, y + totalSize.P2.y / 2);

        Matrix rotMatrix;
        rotMatrix.SetIdentity();
        rotMatrix.Rotate(AxisAngle(0.0f, 0.0f, 1.0f, RAD(degrees)));

        for (int i = 0; i < 4; i++)
        {
            Vect val = coords[i]-Vect(center);
            val.TransformVector(rotMatrix);
            coords[i] = val;
            coords[i] += Vect(center);
        }
    }

    List<UVCoord> &coords = data->UVList[0];
    coords[0].Set(u,  v);
    coords[1].Set(u,  v2);
    coords[2].Set(u2, v);
    coords[3].Set(u2, v2);

    if (!CloseFloat(texDegrees, 0.0f))
    {
        Matrix rotMatrix;
        rotMatrix.SetIdentity();
        rotMatrix.Rotate(AxisAngle(0.0f, 0.0f, 1.0f, -RAD(texDegrees)));

        Vect2 minVal = Vect2(0.0f, 0.0f);
        for (int i = 0; i < 4; i++)
        {
            Vect val = Vect(coords[i]);
            val.TransformVector(rotMatrix);
            coords[i] = val;
            minVal.ClampMax(coords[i]);
        }

        for (int i = 0; i < 4; i++)
            coords[i] -= minVal;
    }

    m_pSpriteVertexBuffer->FlushBuffers();

    LoadVertexBuffer(m_pSpriteVertexBuffer);
    LoadTexture(texture);

    Draw(GS_TRIANGLESTRIP);
}

void D3D10System::DrawBox(const Vect2 &upperLeft, const Vect2 &size)
{
    VBData *data = m_pBoxVertexBuffer->GetData();

    Vect2 bottomRight = upperLeft+size;

    data->VertList[0] = upperLeft;
    data->VertList[1].Set(bottomRight.P2.x, upperLeft.P2.y);
    data->VertList[2].Set(bottomRight.P2.x, bottomRight.P2.y);
    data->VertList[3].Set(upperLeft.P2.x, bottomRight.P2.y);
    data->VertList[4] = upperLeft;

    m_pBoxVertexBuffer->FlushBuffers();

    LoadVertexBuffer(m_pBoxVertexBuffer);

    Draw(GS_LINESTRIP);
}

void D3D10System::ResetViewMatrix()
{
    Matrix4x4Convert(m_curViewMatrix, MatrixStack[curMatrix].GetTranspose());
    Matrix4x4Multiply(m_curViewProjMatrix, m_curViewMatrix, m_curProjMatrix);
    Matrix4x4Transpose(m_curViewProjMatrix, m_curViewProjMatrix);
}

void D3D10System::ResizeView()
{
    LPVOID nullVal = NULL;
    m_pD3d->OMSetRenderTargets(1, (ID3D10RenderTargetView**)&nullVal, NULL);

    SafeRelease(m_pSwapRenderView);

    m_pSwap->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

    ID3D10Texture2D *backBuffer = NULL;
    HRESULT err = m_pSwap->GetBuffer(0, IID_ID3D10Texture2D, (void**)&backBuffer);
    if (FAILED(err))
        CrashError(TEXT("Unable to get back buffer from swap chain"));

    err = m_pD3d->CreateRenderTargetView(backBuffer, NULL, &m_pSwapRenderView);
    if (FAILED(err))
        CrashError(TEXT("Unable to get render view from back buffer"));

    backBuffer->Release();
}

void D3D10System::CopyTexture(Texture *texDest, Texture *texSrc)
{
    D3D10Texture *d3d10Dest = static_cast<D3D10Texture*>(texDest);
    D3D10Texture *d3d10Src  = static_cast<D3D10Texture*>(texSrc);

    m_pD3d->CopyResource(d3d10Dest->m_pTexture, d3d10Src->m_pTexture);
}
