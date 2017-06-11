#include "stdafx.h"
#include "IUtil.h"

using std::to_string;
namespace WUP{
	WCHAR IUtil::s_lpwAppPath[MAX_PATH];
	CHAR IUtil::s_lpAppPath[MAX_PATH];

	void  IUtil::SeedRand()
	{
		srand(GetTickCount());
	}

	bool IUtil::Init()
	{
		GetModuleFileNameW(NULL, s_lpwAppPath, MAX_PATH);
		GetModuleFileNameA(NULL, s_lpAppPath, MAX_PATH);
		WCHAR* wexecutableName = wcsrchr(s_lpwAppPath, L'\\');
		CHAR* executableName = strrchr(s_lpAppPath, '\\');
		executableName[1] = L'\0';
		wexecutableName[1] = '\0';

		return true;
	}

	LPCSTR IUtil::GetAppPathA()
	{
		return s_lpAppPath;
	}

	LPCWSTR IUtil::GetAppPathW()
	{
		return s_lpwAppPath;
	}

	void IUtil::GetText(HWND hwnd, std::string &rStr)
	{
		int len = ::SendMessageA(hwnd, WM_GETTEXTLENGTH, 0, 0);
		char* buffer = new char[len + 1];
		int n = ::SendMessageA(hwnd, WM_GETTEXT, len + 1, (LPARAM)buffer);
		rStr = (LPCSTR)buffer;
		delete[] buffer;
	}

	void IUtil::SetINT(HWND hwnd, int num)
	{
		SendMessageA(hwnd, WM_SETTEXT, NULL, (LPARAM)to_string(num).c_str());
	}

	void IUtil::SetText(HWND hwnd, LPSTR txt)
	{
		SendMessageA(hwnd, WM_SETTEXT, NULL, (LPARAM)txt);
	}

	// Generate random numbers in the half-closed interval
	// [range_min, range_max). In other words,
	// range_min <= random number < range_max
	int IUtil::RangedRand(int range_min, int range_max)
	{
		return (int)((double)rand() / (RAND_MAX + 1) * (range_max - range_min) + range_min);
	}

#define ONEMILLIMETERTOINCH 0.0393701

	void IUtil::AdaptDeviceResolutionFrom96Dpi(POINT &ptTarget, POINT &ptSrc)
	{
		SetProcessDPIAware(); //true
		HDC dcDesk = GetDC(NULL);

		UINT totalScreenHMM = GetDeviceCaps(dcDesk, HORZSIZE);
		UINT totalScreenVMM = GetDeviceCaps(dcDesk, VERTSIZE); // ÆÁÄ»ºÁÃ×Êý

		int fDPIX = GetSystemMetrics(SM_CXSCREEN) / (GetDeviceCaps(dcDesk, HORZSIZE)*ONEMILLIMETERTOINCH);
		int fDPIY = GetSystemMetrics(SM_CYSCREEN) / (GetDeviceCaps(dcDesk, VERTSIZE)*ONEMILLIMETERTOINCH);

		ptTarget.x = (ptSrc.x / 127.0f)*fDPIX;
		ptTarget.y = (ptSrc.y / 127.0f)*fDPIY;

		ReleaseDC(NULL, dcDesk);
	}
};