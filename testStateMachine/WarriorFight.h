#pragma once
#include "InitAttrs.h"
#include "Warrior.h"
#include "CacheBuffer.h"
#include "NetMsgControl.h"
#include "ObjectMngr.h"

namespace WUP{
	class CWarriorFight
	{
	public:
		CWarriorFight();
		virtual ~CWarriorFight();

		bool Init(HWND hwndP1Edit, HWND hwndP2Edit, HWND hwndfightBtn);
		bool GenerateCfg(CInitAttrs &initAttrInst, LPCSTR lpFileName);


		// 模拟客户端发送Fight消息
		bool Fight();
		
		bool Start();	// 服务器逻辑总入口

		void InitNet();	// 初始化网络模块

		void DoHeartBeatObject();

	public:
		HWND m_hwndP1Edit;
		HWND m_hwndP2Edit;
		HWND m_hwndfightBtn;

		CWarrior m_Warrior1;
		CWarrior m_Warrior2;

		WUP::CCacheBuffer* m_pMsgBuffer;	// 模拟网络接口


		// 下面是模拟客户端存在的
		CRITICAL_SECTION m_netCriticalSec;		// 模拟客户端向服务器网络线程之间发送消息的同步对象
		CCacheBuffer m_msgBuffer;				// 模拟客户端向服务器发送

		NetMsgControl m_netMsgControl;				// 网络以及包管理模块
		CRITICAL_SECTION m_sessionCacheCriticalSec;	// 网络线程和服务器逻辑线程之间同步对象
		CCacheBuffer m_sessionCacheBuffer;			// 网络线程和逻辑县城之间的缓存队列 

		//--------------------------------
		BOOL m_isStop = true;					// 服务器是否已经停止

		//-------------------------------- 
		ObjectMngr<CWarrior> m_WarriorMngr;		// 管理所有的Warrior对象
	};
};
