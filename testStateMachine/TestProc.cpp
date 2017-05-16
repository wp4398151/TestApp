#include "stdafx.h"
#include "CDuplicateOutputDx11.h"
#include "TestProc.h"
#include <stdlib.h>
#include <stdio.h>
#include "SystemClass.h"
#include <string>
#include "CaptureAudio.h"
#include "WaveFile.h"
#include <assert.h>
bool MyCopyResource(ID3D10Texture2D **pTargetRenderTexture,
	ID3D10Texture2D *pSrcRenderTexture, ID3D10Device* pDevice, LPCWSTR filePath);

#define CONTENTSIZE (10 * 1000 * 1000)
TestProc::TestProc(SystemClass* pSysClass) :m_pSysClass(pSysClass)
{

}

TestProc::~TestProc()
{

}

void TestProc::Render()
{
	m_Directx10Render.Render();
}

void TestProc::TestDx10(HWND hwnd, POINT& ptTargetSize, POINT& ptCapturePicSize, RECT &rcClipRect)
{
	//// --------------------- SaveToFile 将帧渲染到创建的Texture中
	//IDXGIAdapter* pAdapter = NULL;
	//CDuplicateOutputDx10::GetSpecificAdapter(0, &pAdapter);
	//bool bRet = m_Directx10Render.Init(hwnd, ptTargetSize, rcClipRect, pAdapter);
	//SAFE_RELEASE(pAdapter);
	//CDuplicateOutputDx10 dx10(hwnd, 
	//	m_Directx10Render.m_pd3dDevice, 
	//		m_Directx10Render.m_pSwapChain);
	//bRet = dx10.CreateOutputDuplicator();
	//ID3D10Texture2D *p2dTexture = nullptr;
	//p2dTexture = dx10.GetOneFrame();
	//assert(p2dTexture);

	//ID3D10Texture2D *pTargetRenderTexture = NULL;
	//MyCopyResource(&pTargetRenderTexture, p2dTexture, 
	//	m_Directx10Render.m_pd3dDevice, L"C:\\testbmp\\GetScreen\\");
	//SAFE_RELEASE(pTargetRenderTexture);

	//bRet = m_Directx10Render.CreateTargetViewWithTex(DXGI_FORMAT_B8G8R8A8_UNORM);
	//assert(bRet);
	//bRet = m_Directx10Render.InitModel();
	//assert(bRet);
	//bRet = m_Directx10Render.MapTextrueFromFrame(p2dTexture);
	//assert(bRet);
	//m_Directx10Render.SetTexRenderTarget(false);
	//m_Directx10Render.DrawTexture();
	//m_Directx10Render.SaveToDisk();
	//SAFE_RELEASE(p2dTexture);

	//// ----------------- SaveToSurface 普通资源渲染到创建的texture中
	//IDXGIAdapter* pAdapter = NULL;
	//CDuplicateOutputDx10::GetSpecificAdapter(0, &pAdapter);
	//bool bRet = m_Directx10Render.Init(hwnd, ptTargetSize, rcClipRect, pAdapter);
	//SAFE_RELEASE(pAdapter);
	//bRet = m_Directx10Render.InitModel();
	//assert(bRet);
	//bRet = m_Directx10Render.MapTextrue();
	//assert(bRet);
	//bRet = m_Directx10Render.CreateTargetViewWithTex(DXGI_FORMAT_B8G8R8A8_UNORM);
	//m_Directx10Render.SetTexRenderTarget(false);
	//m_Directx10Render.DrawTexture(false);
	//m_Directx10Render.SaveToDisk();

	//// ------------------ RenderToWindow 渲染到交换链中
	IDXGIAdapter* pAdapter = NULL;
	CDuplicateOutputDx10::GetSpecificAdapter(0, &pAdapter);
	bool bRet = m_Directx10Render.Init(hwnd, ptTargetSize, rcClipRect, pAdapter);
	SAFE_RELEASE(pAdapter);
	bRet = m_Directx10Render.InitModel();
	assert(bRet);
	bRet = m_Directx10Render.MapTextrue();
	assert(bRet);
	m_Directx10Render.SetWindowRenderTarget(false);
	m_Directx10Render.Render();
	m_Directx10Render.CopyFromSwapBackbuffer();
	
	//// --------------------- 直接使用交换链进行渲染
	
	
}

void TestProc::CaptureAudio()
{
	LOG(INFO) << "CaptureAudio Start ...";
	CWaveFile waveFile;

	CaptureAudioClass captureAudioInst(&waveFile);

	if (!captureAudioInst.InitDevice(EDataFlow::eRender, DEVICE_STATE_ACTIVE))
	{
		LOG(ERROR) << "captureAudioClass.InitDevice failed";
		return;
	}

	if (!captureAudioInst.InitFormat())
	{
		LOG(ERROR) << "captureAudioClass.InitFormat failed";
		return;
	}

	string strFile = "d:\\test.wav";
	waveFile.Init(strFile, *captureAudioInst.m_pWaveFormatEx);

	captureAudioInst.Start();
	captureAudioInst.DoRecordLoop();
	captureAudioInst.Stop();
}

void TestProc::fillContent()
{
	for (int i = 0; i < CONTENTSIZE; ++i)
	{
		m_pContent[i] = RangedRand(0, 0x100);
	}
}

// Generate random numbers in the half-closed interval
// [range_min, range_max). In other words,
// range_min <= random number < range_max
int TestProc::RangedRand(int range_min, int range_max)
{
	return (int)((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
}

bool TestProc::TestWriteUDisk()
{
	LOG(INFO) << "Start TestWriteUDisk ...";
	if (m_pContent) free(m_pContent);
	m_pContent = (char*)malloc(CONTENTSIZE);	//10M
	srand(GetTickCount());
	fillContent();

	FILE* pFile = NULL;
	errno_t err = fopen_s(&pFile, "K:\\Test.txt", "w+b");
	UINT curTime = GetTickCount();
	for (int i = 0; i < 5; ++i)
	{
		fwrite(m_pContent, 1,CONTENTSIZE, pFile);
		fflush(pFile);
	}

	LOG(INFO) << (std::to_string(GetTickCount() - curTime).c_str());
	fclose(pFile);
	return true;
}

bool TestProc::TestWriteSSD()
{
	if (m_pContent) free(m_pContent);
	m_pContent = (char*)malloc(CONTENTSIZE);	//10M
	srand(GetTickCount());
	fillContent();

	LOG(INFO) << "Start TestWriteSSD ...";
	FILE* pFile = NULL;
	errno_t err = fopen_s(&pFile, "C:\\Test.txt", "w+b");
	UINT curTime = GetTickCount();
	for (int i = 0; i < 5; ++i)
	{
		fwrite(m_pContent, 1, CONTENTSIZE, pFile);
		fflush(pFile);
	}
	LOG(INFO) << (std::to_string(GetTickCount() - curTime).c_str());
	fclose(pFile);
	return true;
}

bool TestProc::TestWriteHDD()
{
	if (m_pContent) free(m_pContent);
	m_pContent = (char*)malloc(CONTENTSIZE);	//10M
	srand(GetTickCount());
	fillContent();

	LOG(INFO) << ("Start TestWriteHDD ...");
	FILE* pFile = NULL;
	errno_t err = fopen_s(&pFile, "D:\\Test.txt", "w+b");
	UINT curTime = GetTickCount();
	for (int i = 0; i < 5; ++i)
	{
		fwrite(m_pContent, 1, CONTENTSIZE, pFile);
		fflush(pFile);
	}
	
	LOG(INFO) << (std::to_string(GetTickCount() - curTime).c_str());
	fclose(pFile);
	return true;
}