#include "stdafx.h"
#include "WarriorFight.h"
#include "IUtil.h"

namespace WUP{
	CWarriorFight::CWarriorFight()
	{
		m_pMsgBuffer = NULL;
		m_hwndP1Edit = NULL;
		m_hwndP2Edit = NULL;
	}

	CWarriorFight::~CWarriorFight()
	{
	}

	bool CWarriorFight::GenerateCfg(CInitAttrs &initAttrInst, LPCSTR lpFileName)
	{
		string strFilePath = lpFileName;
		strFilePath = string(IUtil::GetAppPathA()) + strFilePath;
		initAttrInst.Init(strFilePath);
		return true;
	}

	bool CWarriorFight::Init(HWND hwndP1Edit, HWND hwndP2Edit, HWND hwndfightBtn)
	{
		m_hwndP1Edit = hwndP1Edit;
		m_hwndP2Edit = hwndP2Edit;
		m_hwndfightBtn = hwndfightBtn;

		CInitAttrs initAttrInst1;
		GenerateCfg(initAttrInst1, "Warrior1.cfg.ini");
		m_Warrior1.Init(&initAttrInst1);

		CInitAttrs initAttrInst2;
		GenerateCfg(initAttrInst2, "Warrior2.cfg.ini");
		m_Warrior2.Init(&initAttrInst2);

		return true;
	}


	bool CWarriorFight::Fight()
	{
		if (m_isStop)
		{
			return true;
		}

		EnterCriticalSection(&m_netCriticalSec);

		FightMsg fightMsg;
		CPacketWriter writer(&m_msgBuffer);
		writer.PutStreamBuffer((LPBYTE)&fightMsg, sizeof(fightMsg));

		LeaveCriticalSection(&m_netCriticalSec);

		return true;
	}

	void CWarriorFight::InitNet()
	{
		InitializeCriticalSection(&m_sessionCacheCriticalSec);
		InitializeCriticalSection(&m_netCriticalSec);

		m_msgBuffer.Init();
		m_sessionCacheBuffer.Init();
		m_netMsgControl.Init(&m_msgBuffer,
			&m_netCriticalSec,
			&m_sessionCacheCriticalSec,
			&m_sessionCacheBuffer, &m_isStop);
		m_netMsgControl.start();
	}

	bool CWarriorFight::Start()
	{
		m_isStop = m_isStop ? false : true;

		if (m_isStop){
			return true;
		}

		// 启动一个线程用来读取，客户端客户端发来的消息
		IUtil::SeedRand();

		TimeManager::Init();
		InitNet();

		while (m_Warrior1.IsAlive() && m_Warrior2.IsAlive())
		{
			TimeManager::RefreshTime();
			if (m_isStop)
			{
				break;
			}

			// 服务器场景事物活动
			DoHeartBeatObject();

			// 抽取msg
			ReturnStatusAsync ret = m_netMsgControl.FetchPacketList();

			// Ai 运行
			if (!m_Warrior2.IsInCD())
			{
				m_Warrior2.Attack(m_Warrior1);
				IUtil::SetINT(m_hwndP1Edit, m_Warrior1.m_curHP);
			}

			// 玩家处理模块
			for (auto pMsg : m_netMsgControl.m_ListMsg){
				if (pMsg->id == EnumFightMsg)
				{
					if (!m_Warrior1.IsInCD())
					{
						m_Warrior1.Attack(m_Warrior2);
						IUtil::SetINT(m_hwndP2Edit, m_Warrior2.m_curHP);
					}
				}
				CPacketParser::ReleaseBuffer((LPBYTE)pMsg);
			}

			// View反馈,当前仅仅只是显示一下玩家的CD剩余
			INT cdShow = m_Warrior1.m_curCD - TimeManager::GetCurTime();
			IUtil::SetINT(m_hwndfightBtn, (cdShow>0)?cdShow:0);

			Sleep(15);
		}

		if (m_Warrior1.IsAlive())
		{
			IUtil::SetText(m_hwndP1Edit, "P1 Win");
			IUtil::SetText(m_hwndP2Edit, "P2 Dead");
		}
		else
		{
			IUtil::SetText(m_hwndP2Edit, "P2 Win");
			IUtil::SetText(m_hwndP1Edit, "P1 Dead");
		}

		if (m_isStop)
		{
			IUtil::SetText(m_hwndP2Edit, "游戏停止");
			IUtil::SetText(m_hwndP1Edit, "游戏停止");
		}
		return true;
	}

	void CWarriorFight::DoHeartBeatObject()
	{
		for (UINT idx : m_WarriorMngr.m_ObjectIdxList)
		{
			// TODO: 做一些和心跳相关的逻辑
		}
	}

};