#include "stdafx.h"
#include "SystemClass.h"
#include "addressbook.pb.h"

SystemClass::SystemClass(void) :LiteThread<SystemClass>(this), m_TestProc(this)
{
	m_applicationName = NULL;
	m_hinstance = NULL;
	m_hwnd = NULL;
	m_Input = NULL;
	m_Test1Btn = NULL;		// 服务器启动
	m_Test2Btn = NULL;		// 服务器停止
	m_Test3Btn = NULL;
	m_Test4Btn = NULL;
	// 设置字体
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LOGFONT));
	lstrcpy(LogFont.lfFaceName, L"宋体");
	LogFont.lfWeight = 400;//FW_NORMAL; 
	LogFont.lfHeight = -13;
	LogFont.lfCharSet = 134;
	LogFont.lfOutPrecision = 3;
	LogFont.lfClipPrecision = 2;
	LogFont.lfQuality = 1;
	LogFont.lfPitchAndFamily = 2;
	// 创建字体 
	m_hFont = CreateFontIndirectW(&LogFont);
}

SystemClass::SystemClass(const SystemClass &) :LiteThread<SystemClass>(this), m_TestProc(this)
{

}

SystemClass::~SystemClass(void)
{
	DeleteObject(m_hFont);

}

//调用窗口初始化函数和其它一些类的初始化函数
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	//创建input对象处理键盘输入
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}
	// 初始化窗口
	InitializeWindows(screenWidth, screenHeight);

	// 初始化输入对象.
	m_Input->Initialize();
	return true;
}

void SystemClass::Shutdown()
{
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}
	// 执行一些销毁工作.
	ShutdownWindows();
}

//处理消息
void  SystemClass::Run()
{
	MSG msg;
	bool done;

	// 初始化消息结构.
	ZeroMemory(&msg, sizeof(MSG));

	// 循环进行消息处理，如果接收到WM_QUIT.
	done = false;
	while(!done)
	{
		// 处理windows消息.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		KeyHandler();
		
		HandleInstant();
		// 接收到WM_QUIT消息，退出程序.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
	}

	return;
}

//初始化窗口类，创建应用程序窗口
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;

	//得到应用程序对象指针
	ApplicationHandle = this;

	// 得到应用程序实例句柄
	m_hinstance = GetModuleHandle(NULL);

	// 应用程序名字
	m_applicationName = L"Engine";

	// 设置窗口类参数.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc; //指定回调函数
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); //默认黑色窗口黑色背景
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);

	// 注册窗口类
	RegisterClassEx(&wc);

	// 窗口模式：800*600.
	screenWidth = 1000;
	screenHeight = 569;

	// 窗口位置,posX, posY窗口左上角坐标
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	RECT clientRect = { 0 };
	GetClientRect(m_hwnd, &clientRect);
	screenHeight = clientRect.bottom - clientRect.top;
	screenWidth = clientRect.right - clientRect.left;

	// 显示窗口并设置其为焦点.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// 隐藏鼠标.
	//ShowCursor(false);
	CreateControls();
}

void SystemClass::ShutdownWindows()
{
	// 销毁窗口
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// 销毁应用程序实例.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;
	ApplicationHandle = NULL;

	return;
}

//
//#pragma comment(lib, "Comdlg32.lib")
//#include <Commdlg.h>
//
//void Choosefont()
//{
//	// Initialize members of the CHOOSEFONT structure.  
//	cf.lStructSize = sizeof(CHOOSEFONT);
//	cf.hwndOwner = (HWND)NULL;
//	cf.hDC = (HDC)NULL;
//	cf.lpLogFont = &lf;
//	cf.iPointSize = 0;
//	cf.Flags = CF_SCREENFONTS;
//	cf.rgbColors = RGB(0, 0, 0);
//	cf.lCustData = 0L;
//	cf.lpfnHook = (LPCFHOOKPROC)NULL;
//	cf.lpTemplateName = (LPWSTR)NULL;
//	cf.hInstance = (HINSTANCE)NULL;
//	cf.lpszStyle = (LPWSTR)NULL;
//	cf.nFontType = SCREEN_FONTTYPE;
//	cf.nSizeMin = 0;
//	cf.nSizeMax = 0;
//	ChooseFont(&cf);
//}

#define CREATEBUTTON(hwndCtrl, caption, ctrlIdx) {															\
	hwndCtrl = CreateWindow(TEXT("button"), caption, \
	WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER, \
	(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd, \
	(HMENU)ctrlIdx, m_hinstance, NULL);																		\
	SendMessage(hwndCtrl, WM_SETFONT, (WPARAM)m_hFont, TRUE);												\
}

#define CREATEEDIT(hwndCtrl, caption, ctrlIdx)	{	\
	hwndCtrl = CreateWindow(TEXT("edit"), caption, \
	WS_CHILD | WS_VISIBLE | WS_BORDER, \
	(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd, \
	(HMENU)(ctrlIdx), m_hinstance, NULL);	\
	SendMessage(hwndCtrl, WM_SETFONT, (WPARAM)m_hFont, TRUE);												\
}

VOID SystemClass::CreateControls()
{
	int marginW = 3;
	int marginH = 3;
	int contrlWidth = 120;
	int contrlHeight= 21;
	int icol = 0;
	int irow = 0;

	m_Test1Btn = CreateWindow(TEXT("button"), TEXT("测试U盘写入"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow *(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest1), m_hinstance, NULL);
	m_Test2Btn = CreateWindow(TEXT("button"), TEXT("测试固态硬盘写入"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest2), m_hinstance, NULL);
	m_Test3Btn = CreateWindow(TEXT("button"), TEXT("测试机械硬盘写入"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest3), m_hinstance, NULL);
	m_Test4Btn = CreateWindow(TEXT("button"), TEXT("DX10绘制"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest4), m_hinstance, NULL);
	m_IPEdit = CreateWindow(TEXT("edit"), TEXT("127.0.0.1"),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexIPEdit), m_hinstance, NULL);
	m_LogClearBtn = CreateWindow(TEXT("button"), TEXT("清 空"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexLogClearBtn), m_hinstance, NULL);
	m_TestDx10RenderBtn  = CreateWindow(TEXT("button"), TEXT("测试Dx10"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_TestDx10Render), m_hinstance, NULL);

	//----------------------------------------------------------------------------------
	++irow;
	icol = 0;

	CREATEEDIT(m_WFP1HP, TEXT("P1"), EIndexCommonCtrl::eCtrlIndex_WFP1HP);
	CREATEEDIT(m_WFP2HP, TEXT("P2"), EIndexCommonCtrl::eCtrlIndex_WFP2HP);
	CREATEBUTTON(m_WFStart, TEXT("游戏开始"), EIndexCommonCtrl::eCtrlIndex_WFTestStart);
	CREATEBUTTON(m_TestFight, TEXT("fight"), EIndexCommonCtrl::eCtrlIndex_TestFight);

	//----------------------------------------------------------------------------------
	++irow;
	icol = 0;

	CREATEBUTTON(m_CaptureAudioBtn, TEXT("抓取音频"), EIndexCommonCtrl::eCtrlIndex_CaptureAudio);
	CREATEBUTTON(m_CaptureScreenIntel, TEXT("抓取屏幕"), EIndexCommonCtrl::eCtrlIndex_CaptureScreenIntel);
	CREATEBUTTON(m_TestCacheBuffer, TEXT("测试缓存"), EIndexCommonCtrl::eCtrlIndex_TestCacheBuffer);
	CREATEBUTTON(m_TestPacketParser, TEXT("测试解包"), EIndexCommonCtrl::eCtrlIndex_TestPacketParser);

	// 取得控件句柄 
	SendMessage(m_Test1Btn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_Test2Btn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_Test3Btn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_Test4Btn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_IPEdit, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_LogClearBtn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
	SendMessage(m_TestDx10RenderBtn, WM_SETFONT, (WPARAM)m_hFont, TRUE);
}

#include <iostream>
#include <fstream>

void SystemClass::testProtoBuf()
{
	tutorial::Person person1;
	person1.set_id(1);
	person1.set_name("hehe");
	person1.set_email("4398151@qq.com");
	int cbSize = person1.ByteSize();
	char* pArrPerson1 = (char*)malloc(cbSize);
	person1.SerializeToArray(pArrPerson1, cbSize);
	tutorial::Person person2;
	person2.ParseFromArray(pArrPerson1, cbSize);
}

void SystemClass::Quit()
{
	PostMessage(m_hwnd, WM_CLOSE, 0, 0);
}

void SystemClass::HandleInstant()
{
	m_TestProc.Render();
}

void SystemClass::KeyHandler()
{
	if (!m_Input)
	{
		Quit();
		return ;
	}

	if (m_Input->IsKeyDown(0x43))		//c:  clear Log
	{
		
	}
	else if(m_Input->IsKeyDown(0x51))	//q: quit
	{
		Quit();
	}
}

unsigned __stdcall SystemClass::RunFunc(void* pThis)
{
	SystemClass* pSystemClass = (SystemClass*)pThis;
	// subwindow msg
	pSystemClass->SubWndMessageHandler(pSystemClass->m_msg_hwnd, pSystemClass->m_msg_umsg, 
										pSystemClass->m_msg_wparam, pSystemClass->m_msg_lparam);
	return 0;
}

void SystemClass::SubWndMessageHandler(HWND hwnd, INT umsg, WPARAM wparam, LPARAM lparam)
{
	if (wparam == EIndexCommonCtrl::eCtrlIndex_indexTest1)
	{
		m_TestProc.TestWriteUDisk();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_indexTest2)
	{
		m_TestProc.TestWriteSSD();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_indexTest3)
	{
		m_TestProc.TestWriteHDD();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_indexTest3)
	{
		m_TestProc.TestWriteHDD();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_indexLogClearBtn)
	{

	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_CaptureScreenIntel)
	{
		// m_CaptureScreenIntel
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_TestPacketParser)
	{
		m_TestProc.TestPacketParser();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_WFTestStart)
	{
		m_TestProc.testGeme(m_WFP1HP, m_WFP2HP, m_TestFight);
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_TestFight)
	{
		m_TestProc.TestFight();
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_CaptureAudio)
	{
		SendMessage(m_CaptureAudioBtn, WM_SETTEXT, NULL, (LPARAM)L"Capturing ...");
		m_TestProc.CaptureAudio();
		SendMessage(m_CaptureAudioBtn, WM_SETTEXT, NULL, (LPARAM)L"抓取音频");
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_TestDx10Render)
	{
		// the size recoder file
		// 最终文件的大小
		POINT ptTargetSize = { 1280, 720};
		//POINT ptTargetSize = { 852, 480};

		// capture file size
		// 原本抓取的图像有多大
		POINT ptCapturePicSize = { 1920, 1080 };
		//POINT ptCapturePicSize = { 852, 480 };

		// clip size
		// 从原图像裁剪之后的区域
		RECT rcClipRect = { 0, 0, 1920, 1080 };

		HWND hwndRender = NULL;
		hwndRender = CreateWindowEx(WS_EX_APPWINDOW, L"static", L"", WS_CLIPCHILDREN | WS_VISIBLE | WS_CHILDWINDOW,
			0, 200, 400, 300, m_hwnd, NULL, m_hinstance, NULL);
		//GetClientRect(hwndRender, &rcClient);
		m_TestProc.TestDx10(hwndRender, ptTargetSize, ptCapturePicSize, rcClipRect);
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_TestCacheBuffer){
		m_TestProc.TestCacheBuffer();
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, INT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
	case WM_KEYDOWN:	// 检测按键消息.
		{
			m_Input->KeyDown((unsigned int)wparam);
			break;
		}
	case WM_KEYUP:
		{
			m_Input->KeyUp((unsigned int)wparam);
			break;
		}
	case WM_SIZE:
		{
			int screenWidth = 0, screenHeight = 0;
			screenWidth = LOWORD(lparam);
			screenHeight = HIWORD(lparam);
			// 窗口大小改变时，重新初始化图形对象 
			break;
		}
	case WM_COMMAND:
	{
			m_msg_hwnd = hwnd;
			m_msg_umsg = umsg;
			m_msg_wparam = wparam;
			m_msg_lparam = lparam;
			start();
		   break;
		}
	//任何其它消息发送到windows缺省处理.
	// 鼠标消息
	case WM_LBUTTONUP:
		{
			 SetFocus(m_hwnd);
		}
		break;
	default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}

	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// 窗口销毁消息.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		// 窗口关闭消息.
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			break;
		}
		//MessageHandle过程处理其它所有消息.
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}