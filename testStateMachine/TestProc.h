#pragma once

#include "DirectXWindow.h"

class SystemClass;

class TestProc
{
public:
	TestProc(SystemClass* pSysClass);
	~TestProc();
	
	bool TestPacketParse();

	bool TestWriteUDisk();
	bool TestWriteSSD();
	bool TestWriteHDD();
	void fillContent();

	void CaptureAudio();
	void Render();
	void TestDx10(HWND hwnd, POINT& ptTargetSize, POINT& ptCapturePicSize, RECT &rcClipRect);
	void testGeme(HWND hwndHP1P, HWND hwndHP2P);
private:	
	CDirectXWindow m_Directx10Render;
	SystemClass* m_pSysClass;
	char* m_pContent;
};