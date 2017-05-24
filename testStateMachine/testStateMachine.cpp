// testStateMachine.cpp : 定义应用程序的入口点。
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

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

//const int INFO = GLOG_INFO, WARNING = GLOG_WARNING,
//ERROR = GLOG_ERROR, FATAL = GLOG_FATAL;


// 此代码模块中包含的函数的前向声明: 
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

	// 创建一个system对象.
	SystemClass* System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// 初始化以及运行system对象.
	bool result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// 关闭以及释放system对象.
	System->Shutdown();
	delete System;
	System = 0;

	google::ShutdownGoogleLogging();
	if (arrArgs){
		LocalFree(arrArgs);
	}

	return 0;
}