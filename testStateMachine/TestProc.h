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
	int RangedRand(int range_min, int range_max);
	void fillContent();

	void CaptureAudio();
	void Render();
	void TestDx10(HWND hwnd, POINT& ptTargetSize, POINT& ptCapturePicSize, RECT &rcClipRect);

private:	
	CDirectXWindow m_Directx10Render;
	SystemClass* m_pSysClass;
	char* m_pContent;
};