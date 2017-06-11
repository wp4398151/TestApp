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
#include "WarriorFight.h"
#include "InitAttrs.h"
#include "IntelScreenCapture.h"
#include "IUtil.h"
#include "CacheBuffer.h"
#include "PacketParser.h"
#include <assert.h>

using WUP::IUtil;

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
	IDXGIAdapter* pAdapter = NULL;
	CDuplicateOutputDx10::GetSpecificAdapter(0, &pAdapter);
	bool bRet = m_Directx10Render.Init(hwnd, ptTargetSize, rcClipRect, pAdapter);
	SAFE_RELEASE(pAdapter);
	CDuplicateOutputDx10 dx10(hwnd, 
		m_Directx10Render.m_pd3dDevice, 
			m_Directx10Render.m_pSwapChain);
	bRet = dx10.CreateOutputDuplicator();
	ID3D10Texture2D *p2dTexture = nullptr;
	p2dTexture = dx10.GetOneFrame();
	assert(p2dTexture);

	if (p2dTexture)
	{
		ALERT("Get Frame SUCCEEDED!");
	}
	else
	{
		ALERT("Get Frame FAILED!");
	}

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
	//IDXGIAdapter* pAdapter = NULL;
	//CDuplicateOutputDx10::GetSpecificAdapter(0, &pAdapter);
	//bool bRet = m_Directx10Render.Init(hwnd, ptTargetSize, rcClipRect, pAdapter);
	//SAFE_RELEASE(pAdapter);
	//bRet = m_Directx10Render.InitModel();
	//assert(bRet);
	//bRet = m_Directx10Render.MapTextrue();
	//assert(bRet);
	//m_Directx10Render.SetWindowRenderTarget(false);
	//m_Directx10Render.Render();
	//m_Directx10Render.CopyFromSwapBackbuffer();
}

void TestProc::TestPacketParser()
{
	string strInfo;
	WUP::CCacheBuffer cache;
	cache.Init();
	WUP::CPacketWriter writer(&cache);

	BYTE pMsg[] = "abcd";
	writer.PutStreamBuffer(pMsg, sizeof(pMsg));

	OutputDebugStringA("\r\n");
	strInfo.clear(); cache.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	OutputDebugStringA("\r\n");

	BYTE pMsg1[] = "1234567";
	writer.PutStreamBuffer(pMsg1, sizeof(pMsg1));

	OutputDebugStringA("\r\n");
	strInfo.clear(); cache.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	OutputDebugStringA("\r\n");

	BYTE pMsg2[] = "1234567890";
	writer.PutStreamBuffer(pMsg2, sizeof(pMsg2));

	OutputDebugStringA("\r\n");
	strInfo.clear(); cache.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	OutputDebugStringA("\r\n");

	WUP::CPacketParser paser(&cache);
	LPBYTE pContent = NULL;
	INT cbRead = 0;

	OutputDebugStringA("----\r\n");
	paser.GetStreamBuffer(&pContent, cbRead);
	OutputDebugStringA((LPSTR)pContent);
	OutputDebugStringA("----\r\n");

	paser.GetStreamBuffer(&pContent, cbRead);
	OutputDebugStringA((LPSTR)pContent);
	OutputDebugStringA("----\r\n");

	paser.GetStreamBuffer(&pContent, cbRead);
	OutputDebugStringA((LPSTR)pContent);
	OutputDebugStringA("----\r\n");

	return;
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

WUP::CWarriorFight g_warriorFightInst;
void TestProc::testGeme(HWND hwndHP1P, HWND hwndHP2P, HWND hwndfightBtn)
{
	g_warriorFightInst.Init(hwndHP1P, hwndHP2P, hwndfightBtn);
	g_warriorFightInst.Start();
}

void TestProc::TestFight()
{
	g_warriorFightInst.Fight();
}

void TestProc::fillContent()
{
	for (int i = 0; i < CONTENTSIZE; ++i)
	{
		m_pContent[i] = IUtil::RangedRand(0, 0x100);
	}
}

// Test Packet Parse Function
bool TestProc::TestPacketParse()
{
	return true;
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

#define TESTSTR1 "123456789012345678901234567890"
#define TESTNUM1 30 

bool TestProc::TestCacheBuffer()
{
	unsigned char bufTestSendMsg[] = TESTSTR1;
	unsigned char bufTestReciveMsg[sizeof(TESTSTR1)] = { 0 };
	unsigned char resultBuf1[TESTNUM1] = {0};
	memcpy(resultBuf1, TESTSTR1 , TESTNUM1);

	WUP::CCacheBuffer cacheBuffer;
	OutputDebugStringA("\r\n Init");
	assert(cacheBuffer.Init());
	string strInfo;
	cacheBuffer.GetInfo(strInfo);
	OutputDebugStringA(strInfo.c_str());

	OutputDebugStringA("\r\n Write");
	assert(cacheBuffer.Write(bufTestSendMsg, sizeof(bufTestSendMsg)));
	strInfo.clear(); cacheBuffer.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	OutputDebugStringA("\r\n WriteSize: ");
	OutputDebugStringA((LPCSTR)std::to_string(sizeof(bufTestSendMsg)).c_str());

	INT cbPeek = 0;
	OutputDebugStringA("\r\n Peek");
	assert(cacheBuffer.Peek(bufTestReciveMsg, TESTNUM1, cbPeek));
	assert((0==memcmp(resultBuf1, bufTestReciveMsg, TESTNUM1)));
	OutputDebugStringA("\r\n PeekContent: ");
	OutputDebugStringA((LPCSTR)bufTestReciveMsg);
	OutputDebugStringA(", PeekSize: ");
	OutputDebugStringA((LPCSTR)std::to_string(cbPeek).c_str());

	strInfo.clear(); cacheBuffer.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());

	for (int i = 0; i < 8; ++i)
	{
		ZeroMemory(bufTestReciveMsg, sizeof(TESTSTR1));
		INT cbRead = 0;
		OutputDebugStringA("\r\n Read");
		assert(cacheBuffer.Read(bufTestReciveMsg, TESTNUM1, cbRead));
		//assert((0 == memcmp(resultBuf1, bufTestReciveMsg, TESTNUM1)));
		OutputDebugStringA("\r\n ReadContent: ");
		OutputDebugStringA((LPCSTR)bufTestReciveMsg);
		OutputDebugStringA(", ReadSize: ");
		OutputDebugStringA((LPCSTR)std::to_string(cbRead).c_str());
		strInfo.clear(); cacheBuffer.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	}

	//ZeroMemory(bufTestReciveMsg, sizeof(TESTSTR1));
	//cbRead = 0;
	//OutputDebugStringA("\r\n Read");
	//assert(cacheBuffer.Read(bufTestReciveMsg, TESTNUM1, cbRead));
	//OutputDebugStringA("\r\n ReadContent: ");
	//OutputDebugStringA((LPCSTR)bufTestReciveMsg);
	//OutputDebugStringA(", ReadSize: ");
	//OutputDebugStringA((LPCSTR)std::to_string(cbRead).c_str());
	//strInfo.clear(); cacheBuffer.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());

	OutputDebugStringA("\r\n Write");
	assert(cacheBuffer.Write(bufTestSendMsg, sizeof(bufTestSendMsg)));
	strInfo.clear(); cacheBuffer.GetInfo(strInfo); OutputDebugStringA(strInfo.c_str());
	OutputDebugStringA("\r\n WriteSize: ");
	OutputDebugStringA((LPCSTR)std::to_string(sizeof(bufTestSendMsg)).c_str());

	return true;
}
