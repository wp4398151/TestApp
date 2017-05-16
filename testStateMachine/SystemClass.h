#pragma once
//����ú��ܹ�����windowsͷ�ļ��Ĵ�С��ʹ������������һЩ����Ҫ���ļ����ӿ����ʱ��
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "InputClass.h"
#include "NetModule.h"
#include "TestProc.h"
#include <string>

class SystemClass
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

	void testProtoBuf();

private:
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	enum EIndexCommonCtrl
	{
		eCtrlIndex_indexTest1 = 1,	// ����U��д��
		eCtrlIndex_indexTest2,		// ���Թ�̬Ӳ��д��
		eCtrlIndex_indexTest3,		// ���Ի�еӲ��д��
		eCtrlIndex_indexTest4,		// ����ʹ��DX10��������Ƶ���һ��Ŀ����
		eCtrlIndex_indexLogClearBtn,
		eCtrlIndex_indexPortEdit,
		eCtrlIndex_indexIPEdit,
		eCtrlIndex_indexMonitorEdit,
		eCtrlIndex_CaptureAudio,	// ץȡ��Ƶ
		eCtrlIndex_TestDx10Render,
	};

	HWND m_Test1Btn;	
	HWND m_Test2Btn;	
	HWND m_Test3Btn;
	HWND m_Test4Btn;
	HWND m_CaptureAudioBtn;		// ������ֹͣ
	HWND m_LogClearBtn;			// ������ֹͣ
	HWND m_TestDx10RenderBtn;	
	HWND m_IPEdit;				// ����IP;
	HFONT m_hFont;

	InputClass* m_Input;		// ��������

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;			// ������

	CNetModule m_NetModule;	// �������
	TestProc m_TestProc;
};

//���徲̬�Ļص������Լ�Ӧ�ó�����
//��Ϊ���崰����ʱ�򣬱���ָ�����ڻص����������������þ�̬�Ļص�����WndProc
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static SystemClass* ApplicationHandle = 0;