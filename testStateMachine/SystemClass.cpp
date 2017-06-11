#include "stdafx.h"
#include "SystemClass.h"
#include "addressbook.pb.h"

SystemClass::SystemClass(void) :LiteThread<SystemClass>(this), m_TestProc(this)
{
	m_applicationName = NULL;
	m_hinstance = NULL;
	m_hwnd = NULL;
	m_Input = NULL;
	m_Test1Btn = NULL;		// ����������
	m_Test2Btn = NULL;		// ������ֹͣ
	m_Test3Btn = NULL;
	m_Test4Btn = NULL;
	// ��������
	LOGFONT LogFont;
	memset(&LogFont, 0, sizeof(LOGFONT));
	lstrcpy(LogFont.lfFaceName, L"����");
	LogFont.lfWeight = 400;//FW_NORMAL; 
	LogFont.lfHeight = -13;
	LogFont.lfCharSet = 134;
	LogFont.lfOutPrecision = 3;
	LogFont.lfClipPrecision = 2;
	LogFont.lfQuality = 1;
	LogFont.lfPitchAndFamily = 2;
	// �������� 
	m_hFont = CreateFontIndirectW(&LogFont);
}

SystemClass::SystemClass(const SystemClass &) :LiteThread<SystemClass>(this), m_TestProc(this)
{

}

SystemClass::~SystemClass(void)
{
	DeleteObject(m_hFont);

}

//���ô��ڳ�ʼ������������һЩ��ĳ�ʼ������
bool SystemClass::Initialize()
{
	int screenWidth = 0, screenHeight = 0;

	//����input�������������
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}
	// ��ʼ������
	InitializeWindows(screenWidth, screenHeight);

	// ��ʼ���������.
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
	// ִ��һЩ���ٹ���.
	ShutdownWindows();
}

//������Ϣ
void  SystemClass::Run()
{
	MSG msg;
	bool done;

	// ��ʼ����Ϣ�ṹ.
	ZeroMemory(&msg, sizeof(MSG));

	// ѭ��������Ϣ����������յ�WM_QUIT.
	done = false;
	while(!done)
	{
		// ����windows��Ϣ.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		KeyHandler();
		
		HandleInstant();
		// ���յ�WM_QUIT��Ϣ���˳�����.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
	}

	return;
}

//��ʼ�������࣬����Ӧ�ó��򴰿�
void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	int posX, posY;

	//�õ�Ӧ�ó������ָ��
	ApplicationHandle = this;

	// �õ�Ӧ�ó���ʵ�����
	m_hinstance = GetModuleHandle(NULL);

	// Ӧ�ó�������
	m_applicationName = L"Engine";

	// ���ô��������.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc; //ָ���ص�����
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH); //Ĭ�Ϻ�ɫ���ں�ɫ����
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);

	// ע�ᴰ����
	RegisterClassEx(&wc);

	// ����ģʽ��800*600.
	screenWidth = 1000;
	screenHeight = 569;

	// ����λ��,posX, posY�������Ͻ�����
	posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	RECT clientRect = { 0 };
	GetClientRect(m_hwnd, &clientRect);
	screenHeight = clientRect.bottom - clientRect.top;
	screenWidth = clientRect.right - clientRect.left;

	// ��ʾ���ڲ�������Ϊ����.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// �������.
	//ShowCursor(false);
	CreateControls();
}

void SystemClass::ShutdownWindows()
{
	// ���ٴ���
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ����Ӧ�ó���ʵ��.
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

	m_Test1Btn = CreateWindow(TEXT("button"), TEXT("����U��д��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow *(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest1), m_hinstance, NULL);
	m_Test2Btn = CreateWindow(TEXT("button"), TEXT("���Թ�̬Ӳ��д��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest2), m_hinstance, NULL);
	m_Test3Btn = CreateWindow(TEXT("button"), TEXT("���Ի�еӲ��д��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest3), m_hinstance, NULL);
	m_Test4Btn = CreateWindow(TEXT("button"), TEXT("DX10����"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexTest4), m_hinstance, NULL);
	m_IPEdit = CreateWindow(TEXT("edit"), TEXT("127.0.0.1"),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexIPEdit), m_hinstance, NULL);
	m_LogClearBtn = CreateWindow(TEXT("button"), TEXT("�� ��"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_indexLogClearBtn), m_hinstance, NULL);
	m_TestDx10RenderBtn  = CreateWindow(TEXT("button"), TEXT("����Dx10"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CENTER,
		(icol++) * (contrlWidth + marginW), irow*(marginH + contrlHeight), contrlWidth, contrlHeight, m_hwnd,
		(HMENU)(EIndexCommonCtrl::eCtrlIndex_TestDx10Render), m_hinstance, NULL);

	//----------------------------------------------------------------------------------
	++irow;
	icol = 0;

	CREATEEDIT(m_WFP1HP, TEXT("P1"), EIndexCommonCtrl::eCtrlIndex_WFP1HP);
	CREATEEDIT(m_WFP2HP, TEXT("P2"), EIndexCommonCtrl::eCtrlIndex_WFP2HP);
	CREATEBUTTON(m_WFStart, TEXT("��Ϸ��ʼ"), EIndexCommonCtrl::eCtrlIndex_WFTestStart);
	CREATEBUTTON(m_TestFight, TEXT("fight"), EIndexCommonCtrl::eCtrlIndex_TestFight);

	//----------------------------------------------------------------------------------
	++irow;
	icol = 0;

	CREATEBUTTON(m_CaptureAudioBtn, TEXT("ץȡ��Ƶ"), EIndexCommonCtrl::eCtrlIndex_CaptureAudio);
	CREATEBUTTON(m_CaptureScreenIntel, TEXT("ץȡ��Ļ"), EIndexCommonCtrl::eCtrlIndex_CaptureScreenIntel);
	CREATEBUTTON(m_TestCacheBuffer, TEXT("���Ի���"), EIndexCommonCtrl::eCtrlIndex_TestCacheBuffer);
	CREATEBUTTON(m_TestPacketParser, TEXT("���Խ��"), EIndexCommonCtrl::eCtrlIndex_TestPacketParser);

	// ȡ�ÿؼ���� 
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
		SendMessage(m_CaptureAudioBtn, WM_SETTEXT, NULL, (LPARAM)L"ץȡ��Ƶ");
	}
	else if (wparam == EIndexCommonCtrl::eCtrlIndex_TestDx10Render)
	{
		// the size recoder file
		// �����ļ��Ĵ�С
		POINT ptTargetSize = { 1280, 720};
		//POINT ptTargetSize = { 852, 480};

		// capture file size
		// ԭ��ץȡ��ͼ���ж��
		POINT ptCapturePicSize = { 1920, 1080 };
		//POINT ptCapturePicSize = { 852, 480 };

		// clip size
		// ��ԭͼ��ü�֮�������
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
	case WM_KEYDOWN:	// ��ⰴ����Ϣ.
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
			// ���ڴ�С�ı�ʱ�����³�ʼ��ͼ�ζ��� 
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
	//�κ�������Ϣ���͵�windowsȱʡ����.
	// �����Ϣ
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
		// ����������Ϣ.
	case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		// ���ڹر���Ϣ.
	case WM_CLOSE:
		{
			PostQuitMessage(0);		
			break;
		}
		//MessageHandle���̴�������������Ϣ.
	default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}