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
		UINT totalScreenVMM = GetDeviceCaps(dcDesk, VERTSIZE); // 屏幕毫米数

		int fDPIX = GetSystemMetrics(SM_CXSCREEN) / (GetDeviceCaps(dcDesk, HORZSIZE)*ONEMILLIMETERTOINCH);
		int fDPIY = GetSystemMetrics(SM_CYSCREEN) / (GetDeviceCaps(dcDesk, VERTSIZE)*ONEMILLIMETERTOINCH);

		ptTarget.x = (ptSrc.x / 127.0f)*fDPIX;
		ptTarget.y = (ptSrc.y / 127.0f)*fDPIY;

		ReleaseDC(NULL, dcDesk);
	}

	UINT TimeManager::s_curTime;					// 当前时间
	UINT TimeManager::s_TimeElapse;					// 当前帧的时间流逝
	UINT TimeManager::s_LastTime;					// 上一帧时间

	void TimeManager::CurrentDateTimeString(string& strTime)
	{
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80] = { 0 };
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%Y-%m-%d, %X: ", &tstruct);
		strTime = buf;
	}

	void TimeManager::CurrentTimeString(string& strTime)
	{
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80] = { 0 };
		localtime_s(&tstruct, &now);
		strftime(buf, sizeof(buf), "%X: ", &tstruct);
		strTime = buf;
	}

	string TimeManager::PrintTime(time_t atime)
	{
		tm timeStruct;
		char buff[32];
		localtime_s(&timeStruct, &atime);
		asctime_s(buff, &timeStruct);
		return string((LPCSTR)buff);
	}

	void TimeManager::GetCurTimeStr(string& strTime, time_t atime)
	{
		tm timeStruct;
		char buff[32];
		localtime_s(&timeStruct, &atime);
		asctime_s(buff, &timeStruct);
		strTime = buff;
	}

	time_t TimeManager::GetOverDayTime()
	{
		time_t curTime;
		time(&curTime);
		tm timeStruct;
		localtime_s(&timeStruct, &curTime);
		timeStruct.tm_hour = 0;
		timeStruct.tm_min = 0;
		timeStruct.tm_sec = 0;
		timeStruct.tm_mday += 1;
		return mktime(&timeStruct);
	}

	time_t TimeManager::GetOverDayTime(time_t curTime)
	{
		tm timeStruct;
		localtime_s(&timeStruct, &curTime);
		timeStruct.tm_hour = 0;
		timeStruct.tm_min = 0;
		timeStruct.tm_sec = 0;
		timeStruct.tm_mday += 1;
		return mktime(&timeStruct);
	}



};