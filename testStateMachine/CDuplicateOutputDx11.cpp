/**
* @file
* @brief it's duplicate the screen, and retain the image.
*
* @author Wupeng <mailto:4398151@qq.com>
*
*/
#include "stdafx.h"
#include "CDuplicateOutputDx11.h"
#include <assert.h>
#include <string>

using namespace std;
	//TipMsgWindow tip;
	//POINT pos = { 1, 1 };
	//tip.Init("test Output", pos, pos);
	//tip.ShowATopMostInfoWindow();
	//HWND hwnd = tip.GetWnd();
	//CDuplicateOutputDx11 output(hwnd);
	//do
	//{
	//	DOLOG("Start****************");
	//	output.close();
	//	output.Release();
	//	output.Init();
	//	output.start();
	//	WaitForSingleObject(output.m_hThread, INFINITE);
	//} while (output.GetStatus() == DupicatorStatus_Restart);
	//DOLOG("Exit******************");

void CreateBMPFileEx(LPTSTR pszFile, PBITMAPINFO pbi, LPBYTE lpBits)
{
	HANDLE hf = NULL;                 // file handle  
	BITMAPFILEHEADER hdr = { 0 };       // bitmap file-header  
	PBITMAPINFOHEADER pbih = NULL;    // bitmap info-header  
	DWORD dwTotal = 0;				// total count of bytes  
	DWORD cb = 0;                   // incremental count of bytes  
	BYTE *hp = 0;                   // byte pointer  
	DWORD dwTmp = 0;

	pbih = (PBITMAPINFOHEADER)pbi;

	// Retrieve the color table (RGBQUAD array) and the bits  
	// (array of palette indices) from the DIB.  
	// Create the .BMP file.  
	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD)0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);
	assert(hf != INVALID_HANDLE_VALUE);
	hdr.bfType = 0x4d42;        // 0x42 = "B"  0x4d = "M"  
	// Compute the size of the entire file.  
	hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER)+
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD)+pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	// Compute the offset to the array of color indices.  
	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER)+
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD);

	// Copy the BITMAPFILEHEADER into the .BMP file.  
	BOOL res = WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD)&dwTmp, NULL);
	assert(res != 0);

	// Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
	WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
		+pbih->biClrUsed * sizeof(RGBQUAD),
		(LPDWORD)&dwTmp, (NULL));

	// Copy the array of color indices into the .BMP file.  
	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL);

	// Close the .BMP file.  
	CloseHandle(hf);
}

void WriteCaptureSpecificDCRGBbitsEx(LPBYTE lpBits, LPTSTR lpFilePath, UINT width, UINT height, UINT pixelBitSize)
{
	BITMAPINFO bmpinfo = { 0 };

	bmpinfo.bmiHeader.biBitCount = pixelBitSize * 8;//颜色位数
	bmpinfo.bmiHeader.biClrImportant = 0;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biHeight = height;
	bmpinfo.bmiHeader.biWidth = width;
	bmpinfo.bmiHeader.biPlanes = 1; //级别?必须为1
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biSizeImage = height*width*pixelBitSize;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmpinfo.bmiHeader.biYPelsPerMeter = 0;

	CreateBMPFileEx(lpFilePath, &bmpinfo, lpBits);
}

bool MyCopyResource(ID3D10Texture2D **pTargetRenderTexture,
	ID3D10Texture2D *pSrcRenderTexture, ID3D10Device* pDevice, LPCWSTR filePath)
{
	if (pTargetRenderTexture == NULL || pSrcRenderTexture == NULL)
	{
		return false;
	}

	IDXGIKeyedMutex *pMutex = NULL;
	HRESULT hr = S_OK;

	D3D10_TEXTURE2D_DESC renderDesc;
	pSrcRenderTexture->GetDesc(&renderDesc);
	D3D10_TEXTURE2D_DESC copyDesc;
	ZeroMemory(&copyDesc, sizeof(&copyDesc));
	copyDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	copyDesc.Format = renderDesc.Format;
	copyDesc.Height = renderDesc.Height;
	copyDesc.Width = renderDesc.Width;
	copyDesc.Usage = D3D10_USAGE_STAGING;
	copyDesc.ArraySize = 1;
	copyDesc.SampleDesc.Count = 1;
	copyDesc.SampleDesc.Quality = 0;
	copyDesc.MipLevels = 1;
	copyDesc.MiscFlags = 0;//D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	copyDesc.BindFlags = 0;//D3D11_BIND_SHADER_RESOURCE;
	D3D10_SUBRESOURCE_DATA *lpSRD = NULL;

	hr = pDevice->CreateTexture2D(&copyDesc, lpSRD, pTargetRenderTexture);
	if (FAILED(hr))
	{
		return false;
	}
	pDevice->CopyResource((ID3D10Resource*)*pTargetRenderTexture, (ID3D10Resource*)pSrcRenderTexture);

	HRESULT result = S_OK;
	D3D10_MAPPED_TEXTURE2D map;

	//---------------------
	if (SUCCEEDED(result = (*pTargetRenderTexture)->Map(0, D3D10_MAP_READ, 0, &map)))
	{
		wstring ss;
		//ss += L"f:\\testbmp\\";
		ss += filePath;
		ss += to_wstring(GetTickCount());
		ss += L".bmp";
		WriteCaptureSpecificDCRGBbitsEx((LPBYTE)map.pData, (LPWSTR)ss.c_str(), renderDesc.Width, renderDesc.Height, 4);
		(*pTargetRenderTexture)->Unmap(0);
	}
	//---------------------

	return true;
}

void CDuplicateOutputDx10::Release()
{
	SAFE_RELEASE(m_pOutputDuplication);
}

void CDuplicateOutputDx10::DoneWithFrame()
{
	m_pOutputDuplication->ReleaseFrame();
}

DuplicatorInfo CDuplicateOutputDx10::GetNewFrame(ID3D10Texture2D** p2dTexture)
{
	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	IDXGIResource* pDesktopResource = nullptr;
	HRESULT hr = S_OK;
	hr = m_pOutputDuplication->AcquireNextFrame(500, &frameInfo, &pDesktopResource);
	if (hr == DXGI_ERROR_ACCESS_LOST)
	{
		return DuplicatorInfo_Lost;
	}
	else if (hr == DXGI_ERROR_WAIT_TIMEOUT)
	{
		return DuplicatorInfo_Timeout;
	}
	else if (hr == DXGI_ERROR_INVALID_CALL)
	{
		return DuplicatorInfo_InvalidCall;
	}
	else if (FAILED(hr))
	{
		return DuplicatorInfo_Error;
	}

	if (FAILED(hr = pDesktopResource->QueryInterface(__uuidof(ID3D10Texture2D), reinterpret_cast<void**>(p2dTexture))))
	{
		SAFE_RELEASE(pDesktopResource);
		DOLOG(" pDesktopResource->QueryInterface failed");
		return DuplicatorInfo_Error;
	}
	
	SAFE_RELEASE(pDesktopResource);
	return DuplicatorInfo_Acquired;
}

ID3D10Texture2D* CDuplicateOutputDx10::GetOneFrame()
{
	ID3D10Texture2D* p2dTexture = nullptr;
	ID3D10Texture2D* p2dRetTexture = nullptr;
	DuplicatorInfo retInfo = GetNewFrame(&p2dTexture);
	D3D10_TEXTURE2D_DESC desc2D;
	if (retInfo == DuplicatorInfo_Acquired)
	{
		p2dTexture->GetDesc(&desc2D);
		p2dRetTexture = SaveTextureInBMP(p2dTexture);
		SAFE_RELEASE(p2dTexture);
		DoneWithFrame();
		return p2dRetTexture;
	}
	LOG(ERROR) << "GetNewFrame Failed! " << retInfo;
	return NULL;
}

unsigned __stdcall CDuplicateOutputDx10::RunFunc(void* pInst)
{
	assert(pInst);
	CDuplicateOutputDx10* pThis = reinterpret_cast<CDuplicateOutputDx10*>(pInst);
	if (!pThis->CreateOutputDuplicator())
	{
		pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
		return 0;
	}

	DWORD startCount = 0;
	DWORD curTime = 0;
	startCount = GetTickCount();
	while (true)
	{
		ID3D10Texture2D* p2dTexture = nullptr;

		pThis->DoneWithFrame();
		DuplicatorInfo retInfo = pThis->GetNewFrame(&p2dTexture);

		D3D10_TEXTURE2D_DESC desc2D;
		switch (retInfo)
		{
		case DuplicatorInfo_Acquired:
		{
			p2dTexture->GetDesc(&desc2D);
			pThis->SaveTextureInBMP(p2dTexture);
			DOLOG("GetNewFrame Lapse Acquired : " << (GetTickCount() - startCount) << "mili sec, x:" << desc2D.Width << ", y:" << desc2D.Height);
			startCount = GetTickCount();
			SAFE_RELEASE(p2dTexture);
			continue;
		}
		case DuplicatorInfo_Lost:
		{
			DOLOG("GetNewFrame Lapse Lost: " << (GetTickCount() - startCount) << "mili sec------------------------------------------------------------ \r\n");
			startCount = GetTickCount();
			if (false == pThis->CreateOutputDuplicator())
			{
				pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
				DOLOG("GetNewFrame Lapse Acquired End Restart");
				return 0;
			}
			DOLOG("GetNewFrame Lapse Acquired Continue");
			continue;
		}
		case DuplicatorInfo_InvalidCall:
		{
			DOLOG("GetNewFrame Lapse invalid call: "<< (GetTickCount() - startCount) << "mili sec<<<<<<<<< \r\n");
			startCount = GetTickCount();
			pThis->m_DuplicatorStatus = DupicatorStatus_Restart;
			return 0;
		}
		case DuplicatorInfo_Timeout:
		{
			//pThis->DoneWithFrame();
			//DOLOG("GetNewFrame Lapse Timeout: " + (GetTickCount() - startCount) + "mili sec \r\n");
			continue;
		}
		case DuplicatorInfo_Error:
		default:
		{
			startCount = GetTickCount();
			DOLOG("GetNewFrame Lapse Error: " << (GetTickCount() - startCount) << "mili sec \r\n");
			pThis->m_DuplicatorStatus = DupicatorStatus_Exit;
			return 0;
		}
		};
	}
	return 0;
}

bool CDuplicateOutputDx10::CopyResource(ID3D10Texture2D **pTargetRenderTexture, ID3D10Texture2D *pSrcRenderTexture)
{
	if (pTargetRenderTexture == NULL || pSrcRenderTexture == NULL)
	{
		return false;
	}

	IDXGIKeyedMutex *pMutex = NULL;
	HRESULT hr = S_OK;

	D3D10_TEXTURE2D_DESC renderDesc;
	pSrcRenderTexture->GetDesc(&renderDesc);

	D3D10_TEXTURE2D_DESC copyDesc;
	ZeroMemory(&copyDesc, sizeof(&copyDesc));
	copyDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
	copyDesc.Format = renderDesc.Format;
	copyDesc.Height = renderDesc.Height;
	copyDesc.Width = renderDesc.Width;
	copyDesc.Usage = D3D10_USAGE_DEFAULT;
	copyDesc.ArraySize = 1;
	copyDesc.SampleDesc.Count = 1;
	copyDesc.SampleDesc.Quality = 0;
	copyDesc.MipLevels = 1;
	copyDesc.MiscFlags = 0;//D3D10_RESOURCE_MISC_SHARED_KEYEDMUTEX;
	copyDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	D3D10_SUBRESOURCE_DATA *lpSRD = NULL;

	hr = m_pDevice->CreateTexture2D(&copyDesc, lpSRD, pTargetRenderTexture);
	if (FAILED(hr))
	{
		return false;
	}

	m_pDevice->CopyResource((ID3D10Resource*)*pTargetRenderTexture, (ID3D10Resource*)pSrcRenderTexture);
		
	return true;
}

ID3D10Texture2D* CDuplicateOutputDx10::SaveTextureInBMP(ID3D10Texture2D* p2dTexture)
{
	if (!p2dTexture){
		return NULL;
	}

	ID3D10Texture2D* pTextureBuf = NULL;
	if (!CopyResource(&pTextureBuf, p2dTexture))
	{
		return NULL;
	}

	/*ID3D10Texture2D *pTargetRenderTexture = NULL;
	MyCopyResource(&pTargetRenderTexture, pTextureBuf, m_pDevice, L"C:\\testbmp\\GetScreen\\");
	if (pTargetRenderTexture)
	{
		pTargetRenderTexture->Release();
	}*/
	return pTextureBuf;
	//SAFE_RELEASE(pTextureBuf);
}

bool CDuplicateOutputDx10::CreateOutputDuplicator()
{
	SAFE_RELEASE(m_pOutputDuplication);

	HRESULT hRes = S_OK;
	IDXGIDevice* pDxgiDevice = nullptr;

	hRes = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (!SUCCEEDED(hRes))
	{
		DOLOG("m_pDevice->QueryInterface failed!");
		return false;
	}
	
	IDXGIAdapter* pDxgiAdapter = nullptr;
	hRes = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	SAFE_RELEASE(pDxgiDevice);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiDevice->GetParent failed!");
		return false;
	}
	
	DXGI_ADAPTER_DESC descAdapter;
	pDxgiAdapter->GetDesc(&descAdapter);

	// Get output
	IDXGIOutput* pDxgiOutput = nullptr;
	hRes = pDxgiAdapter->EnumOutputs(0, &pDxgiOutput);
	SAFE_RELEASE(pDxgiAdapter);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiAdapter->EnumOutputs failed!");
		return false;
	}

	// Get output1
    IDXGIOutput1* pDxgiOutput1 = nullptr;
    hRes = pDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), reinterpret_cast<void**>(&pDxgiOutput1));
	SAFE_RELEASE(pDxgiOutput);
    if (!SUCCEEDED(hRes))
    {
		DOLOG("pDxgiOutput->QueryInterface failed!");
		return false;
    }
	
	// Get duplicate
	hRes = pDxgiOutput1->DuplicateOutput(m_pDevice, &m_pOutputDuplication);
	SAFE_RELEASE(pDxgiOutput1);
	if (!SUCCEEDED(hRes))
	{
		DOLOG("pDxgiOutput1->DuplicateOutput");
		return false;
	}
		
	return true;
}

bool CDuplicateOutputDx10::GetSpecificAdapter(int idAdapter, IDXGIAdapter** pAdapter)
{
	HRESULT err = S_OK;

	if (!pAdapter)
	{
		return false;
	}

	REFIID iidVal = __uuidof(IDXGIFactory1);
	UINT adapterID = 0;	// adapter index
	IDXGIFactory1* pFactory = NULL;
	if (FAILED(err = CreateDXGIFactory1(iidVal, (void**)&pFactory)))
	{
		return 	false;
	}
	UINT i = 0;
	UINT adapterDeviceID = idAdapter;		// if device id equal zero, use the first device

	DXGI_ADAPTER_DESC dxgiDesc;
	IDXGIAdapter1 *giAdapter = NULL;
	if (pFactory->EnumAdapters1(i, &giAdapter) != S_OK)
	{
		return false;
	}
	DXGI_ADAPTER_DESC desc;
	giAdapter->GetDesc(&desc);
	if (pFactory)pFactory->Release();
	*pAdapter = giAdapter;
	return true;
}

bool CDuplicateOutputDx10::Init()
{
	//HRESULT err = NULL;
	//DXGI_SWAP_CHAIN_DESC swapDesc;
	//ZeroMemory(&swapDesc, sizeof(swapDesc));
	//swapDesc.BufferCount = 2;
	//swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//swapDesc.BufferDesc.Width = 1920;
	//swapDesc.BufferDesc.Height = 1080;
	//swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swapDesc.OutputWindow = m_hwnd;
	//swapDesc.SampleDesc.Count = 1;
	//swapDesc.Windowed = TRUE;
	//D3D_FEATURE_LEVEL desiredLevels[6] =
	//{
	//	D3D_FEATURE_LEVEL_11_0,
	//	D3D_FEATURE_LEVEL_10_1,
	//	D3D_FEATURE_LEVEL_10_0,
	//	D3D_FEATURE_LEVEL_9_3,
	//	D3D_FEATURE_LEVEL_9_2,
	//	D3D_FEATURE_LEVEL_9_1,
	//};
	//D3D_FEATURE_LEVEL receivedLevel;
	//D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
	//UINT flags = 0;//D3D11_CREATE_DEVICE_DEBUG;
	////err = D3D11CreateDeviceAndSwapChain(pAdapter, driverType, NULL, flags, desiredLevels, 6, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, &m_pDevice, &receivedLevel, &m_pDeviceContext);
	//IDXGIAdapter* pAdapter = NULL;
	//GetSpecificAdapter(0, &pAdapter);
	//DXGI_ADAPTER_DESC adapterDesc;
	//pAdapter->GetDesc(&adapterDesc);
	//
	//err = D3D11CreateDeviceAndSwapChain(pAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, 
	//					flags, desiredLevels, 6, D3D11_SDK_VERSION, &swapDesc, &m_pSwapChain, 
	//						&m_pDevice, &receivedLevel, &m_pDeviceContext);
	//if (SUCCEEDED(err))
	//{
	//	//m_pSwapChain->AddRef();
	//	//m_pDevice->AddRef(); 
	//	//m_pDeviceContext->AddRef();
	//	return true;
	//}
	//return false;
	return true;
}