#pragma once
#include <string>
#include <process.h>

using std::string;

namespace WUP{

#define ALERT(msg) MessageBox(NULL, TEXT("info"), TEXT(msg), MB_OK)

	class IUtil
	{
	public:
		static bool Init();
		static LPCSTR GetAppPathA();
		static LPCWSTR GetAppPathW();
		static void GetText(HWND hwnd, string &rStr);
		static void SetINT(HWND hwnd, int num);
		static void SetText(HWND hwnd, LPSTR txt);
		static void SeedRand();
		static int RangedRand(int range_min, int range_max);
		static WCHAR s_lpwAppPath[MAX_PATH];
		static CHAR s_lpAppPath[MAX_PATH];
		static void AdaptDeviceResolutionFrom96Dpi(POINT &ptTarget, POINT &ptSrc);
	};

	////////////////////////////////////////////////////
	// 线程类
	// Wupeng 
	////////////////////////////////////////////////////
	template<typename T>
	class LiteThread
	{
	public:
		LiteThread(T *pInst);
		~LiteThread();

		BOOL close();
		BOOL start();

	public:
		T *m_pInst;
		HANDLE m_hThread;
		unsigned int m_threadID;
	};

	template<typename T>
	LiteThread<T>::LiteThread(T *pInst)
	{
		m_pInst = pInst;
		m_hThread = 0;
		m_threadID = 0;
	}

	template<typename T>
	LiteThread<T>::~LiteThread()
	{
		m_pInst = NULL;
		m_hThread = 0;
		m_threadID = 0;
	}

	template<typename T>
	BOOL LiteThread<T>::close()
	{
		if (!m_hThread)
		{
			return TRUE;
		}
		if (0 == WaitForSingleObject(m_hThread, 0))
		{
			if (CloseHandle(m_hThread))
			{
				m_pInst = NULL;
				m_hThread = 0;
				m_threadID = 0;
				return TRUE;
			}
		}
		return FALSE;
	}

	template<typename T>
	BOOL LiteThread<T>::start()
	{
		m_hThread = (HANDLE)_beginthreadex(NULL, 0, T::RunFunc, (T*)this, 0, &m_threadID);
		if (0 == m_hThread)
		{
			return FALSE;
		}
		return TRUE;
	}

	enum ReturnStatusAsync
	{
		Success = 0,	// 成功调用
		Fatal = 1,		// 致命错误,可能需要停止掉整个正序，或者停止当前会话
		Skip = 2,		// 忽略本次调用结果,可能出现错误，但不是严重错误
	};

	////////////////////////////////////////////////////
	// 时间管理
	// Wupeng 
	////////////////////////////////////////////////////
	class TimeManager
	{
	public:
		static UINT s_curTime;						// 当前时间
		static UINT s_TimeElapse;					// 当前帧的时间流逝
		static UINT s_LastTime;						// 上一帧时间

		static inline void Init()
		{
			s_curTime = GetTickCount();
			s_TimeElapse = 0;
			s_LastTime = s_curTime;
		}

		static inline void RefreshTime()
		{
			s_LastTime = s_curTime;
			s_curTime = GetTickCount();				// 当前时间
			s_TimeElapse = s_curTime - s_LastTime;	// 当前帧的时间流逝
		}

		static inline UINT GetCurTime()
		{
			return s_curTime;
		}

		static inline UINT GetCurElapseTime()
		{
			return s_TimeElapse;
		}

		// 得到未来时间
		static time_t GetOverDayTime(time_t curTime);
		static time_t GetOverDayTime();
		
		static string PrintTime(time_t atime);
		static void GetCurTimeStr(string& strTime, time_t atime);
		static void CurrentDateTimeString(string& strTime);
		static void CurrentTimeString(string& strTime);
	};
}