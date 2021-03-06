#pragma once
//定义该宏能够减少windows头文件的大小，使编译器不编译一些不必要的文件，加快编译时间
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "InputClass.h"
#include "NetModule.h"
#include "TestProc.h"
#include <string>
#include "IUtil.h"

class SystemClass : public WUP::LiteThread<SystemClass>
{
public:
	SystemClass(void);
	SystemClass(const SystemClass &);
	~SystemClass(void);

	bool Initialize();
	void Shutdown();
	void Run();
	void MoveCamera();
	void CreateControls();
	void KeyHandler();
	void Quit();
	void HandleInstant();
	
	LRESULT MessageHandler(HWND hwnd, INT umsg, WPARAM wparam, LPARAM lparam);
	void SubWndMessageHandler(HWND hwnd, INT umsg, WPARAM wparam, LPARAM lparam);
	
	HWND m_msg_hwnd;
	INT m_msg_umsg;
	WPARAM m_msg_wparam;
	LPARAM m_msg_lparam;

	void testProtoBuf();

	static unsigned __stdcall RunFunc(void* pThis);

private:
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	enum EIndexCommonCtrl
	{
		eCtrlIndex_indexTest1 = 1,	// 测试U盘写入
		eCtrlIndex_indexTest2,		// 测试固态硬盘写入
		eCtrlIndex_indexTest3,		// 测试机械硬盘写入
		eCtrlIndex_indexTest4,		// 测试使用DX10将纹理绘制到另一个目标上
		eCtrlIndex_indexLogClearBtn,
		eCtrlIndex_indexPortEdit,
		eCtrlIndex_indexIPEdit,
		eCtrlIndex_indexMonitorEdit,
		eCtrlIndex_CaptureAudio,		// 抓取音频
		eCtrlIndex_TestDx10Render,
		eCtrlIndex_WFTestStart,			// Warrior Fight test start
		eCtrlIndex_WFP1HP,				// Warrior Fight HP of P1
		eCtrlIndex_WFP2HP,				// Warrior Fight HP of P2
		eCtrlIndex_CaptureScreenIntel,	// 抓取屏幕
		eCtrlIndex_TestCacheBuffer,		// 测试CacheBuffer
		eCtrlIndex_TestPacketParser,	// 测试PacketParser
		eCtrlIndex_TestFight,			// 测试手动攻击
	};

	HWND m_Test1Btn;	
	HWND m_Test2Btn;	
	HWND m_Test3Btn;
	HWND m_Test4Btn;
	HWND m_CaptureAudioBtn;		// 服务器停止
	HWND m_LogClearBtn;			// 服务器停止
	HWND m_TestDx10RenderBtn;	
	HWND m_IPEdit;				// 链接IP;
	HFONT m_hFont;
	HWND m_WFP1HP;				
	HWND m_WFP2HP;
	HWND m_WFStart;				
	HWND m_CaptureScreenIntel;
	HWND m_TestCacheBuffer;
	HWND m_TestPacketParser;
	HWND m_TestFight;			// 手动攻击

	InputClass* m_Input;		// 键盘输入

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;			// 主窗口

	CNetModule m_NetModule;	// 网络核心
	TestProc m_TestProc;
};

//定义静态的回调函数以及应用程序句柄
//因为定义窗口类时候，必须指定窗口回调函数，所以我们用静态的回调函数WndProc
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;