/**
 * @file
 * @brief it's duplicate the screen, and retain the image.
 *
 * @author Wupeng <mailto:4398151@qq.com>
 *
 */
#pragma once
#include <Windows.h>
#include <dxgi1_2.h>
#include <D3D10_1.h>
#include <dxgi.h>
#include <D3D10.h>

enum DuplicatorInfo
{
	DuplicatorInfo_Error,		// need to stop recorder
	DuplicatorInfo_Timeout,		// need re acquire
	DuplicatorInfo_Lost,		// need recreate the duplication
	DuplicatorInfo_Acquired,	// succeeded
	DuplicatorInfo_InvalidCall
};

enum DupicatorStatus
{
	DupicatorStatus_Restart,
	DupicatorStatus_Exit,
};

/**
 * Main class for management the source of duplication
 */
class CDuplicateOutputDx10
{
public:
	CDuplicateOutputDx10(HWND hwnd, ID3D10Device* pDevice, IDXGISwapChain* pSwapChain) : 
		m_pSwapChain(pSwapChain), m_pDevice(pDevice), m_pOutputDuplication(nullptr), m_hwnd(hwnd) {
	}
	bool Init();
	void Release();
	bool CreateOutputDuplicator();
	void DoneWithFrame();
	DuplicatorInfo GetNewFrame(ID3D10Texture2D** p2dTexture);
	static unsigned __stdcall RunFunc(void* pInst);
	ID3D10Texture2D* GetOneFrame();
	DupicatorStatus GetStatus(){ return m_DuplicatorStatus; }

	ID3D10Texture2D* SaveTextureInBMP(ID3D10Texture2D* p2dTexture);
	bool CopyResource(ID3D10Texture2D **pTargetRenderTexture, ID3D10Texture2D *pSrcRenderTexture);
	static bool GetSpecificAdapter(int idAdapter, IDXGIAdapter** pAdapter);
	DupicatorStatus m_DuplicatorStatus;
protected:
	HWND m_hwnd;
	IDXGISwapChain* m_pSwapChain;
	ID3D10Device* m_pDevice;
	IDXGIOutputDuplication* m_pOutputDuplication;
};
