// testStateMachine.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "testStateMachine.h"
#include "SystemClass.h"
#include <shellapi.h>
#include <string>
#include "IUtil.h"

#define MAX_LOADSTRING 100



#define LOGINFOFILE	"Log\\Info"
#define LOGWARNFILE	"Log\\Warn"
#define LOGERRFILE	"Log\\Err"
#define LOGFATAL	"Log\\Fatl"

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������

//const int INFO = GLOG_INFO, WARNING = GLOG_WARNING,
//ERROR = GLOG_ERROR, FATAL = GLOG_FATAL;


// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	
	LPCSTR lpCommandLine = GetCommandLineA();
	char lpCommandLineBuf[MAX_PATH] = { 0 };
	strcpy_s((char*)lpCommandLineBuf, MAX_PATH, lpCommandLine);
	(*strchr(lpCommandLineBuf, ' '))= 0;
	LPCWSTR lpwCommandLine = GetCommandLineW();
	INT argc = 0;
	LPWSTR* arrArgs = CommandLineToArgvW(lpwCommandLine, &argc);
	google::InitGoogleLogging(lpCommandLineBuf);
	
	IUtil::Init();
	
	google::SetLogDestination(google::GLOG_INFO, (std::string((LPCSTR)IUtil::GetAppPathA()) + LOGINFOFILE).c_str());
	google::SetLogDestination(google::GLOG_WARNING, (std::string((LPCSTR)IUtil::GetAppPathA()) + LOGWARNFILE).c_str());
	google::SetLogDestination(google::GLOG_ERROR, (std::string((LPCSTR)IUtil::GetAppPathA()) + LOGERRFILE).c_str());
	google::SetLogDestination(google::GLOG_FATAL, (std::string((LPCSTR)IUtil::GetAppPathA()) + LOGFATAL).c_str());
	
	LOG(INFO) << "\r\n *********** Start... **************";

	// ����һ��system����.
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// ��ʼ���Լ�����system����.
	bool result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// �ر��Լ��ͷ�system����.
	System->Shutdown();
	delete System;
	System = 0;

	google::ShutdownGoogleLogging();
	if (arrArgs){
		LocalFree(arrArgs);
	}

	return 0;
}