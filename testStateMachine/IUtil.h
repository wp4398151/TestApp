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
	// �߳���
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
		Success = 0,	// �ɹ�����
		Fatal = 1,		// ��������,������Ҫֹͣ���������򣬻���ֹͣ��ǰ�Ự
		Skip = 2,		// ���Ա��ε��ý��,���ܳ��ִ��󣬵��������ش���
	};

	////////////////////////////////////////////////////
	// ʱ�����
	// Wupeng 
	////////////////////////////////////////////////////
	class TimeManager
	{
	public:
		static UINT s_curTime;						// ��ǰʱ��
		static UINT s_TimeElapse;					// ��ǰ֡��ʱ������
		static UINT s_LastTime;						// ��һ֡ʱ��

		static inline void Init()
		{
			s_curTime = GetTickCount();
			s_TimeElapse = 0;
			s_LastTime = s_curTime;
		}

		static inline void RefreshTime()
		{
			s_LastTime = s_curTime;
			s_curTime = GetTickCount();				// ��ǰʱ��
			s_TimeElapse = s_curTime - s_LastTime;	// ��ǰ֡��ʱ������
		}

		static inline UINT GetCurTime()
		{
			return s_curTime;
		}

		static inline UINT GetCurElapseTime()
		{
			return s_TimeElapse;
		}

		// �õ�δ��ʱ��
		static time_t GetOverDayTime(time_t curTime);
		static time_t GetOverDayTime();
		
		static string PrintTime(time_t atime);
		static void GetCurTimeStr(string& strTime, time_t atime);
		static void CurrentDateTimeString(string& strTime);
		static void CurrentTimeString(string& strTime);
	};
}