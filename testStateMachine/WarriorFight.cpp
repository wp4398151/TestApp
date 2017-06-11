#include "stdafx.h"
#include "WarriorFight.h"
#include "IUtil.h"
#include "NetMsgControl.h"

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

	CRITICAL_SECTION g_netCriticalSec;
	CCacheBuffer g_msgBuffer;
	BOOL g_isStop = true;

	bool CWarriorFight::Fight()
	{
		if (g_isStop)
		{
			return true;
		}

		EnterCriticalSection(&g_netCriticalSec);

		FightMsg fightMsg;
		CPacketWriter writer(&g_msgBuffer);
		writer.PutStreamBuffer((LPBYTE)&fightMsg, sizeof(fightMsg));

		LeaveCriticalSection(&g_netCriticalSec);

		return true;
	}

	bool CWarriorFight::Start()
	{
		g_isStop = g_isStop ? false : true;

		if (g_isStop){
			return true;
		}

		// ����һ���߳�������ȡ
		IUtil::SeedRand();

		NetMsgControl netMsgControl;

		CRITICAL_SECTION sessionCacheCriticalSec;
		CCacheBuffer sessionCacheBuffer;

		InitializeCriticalSection(&sessionCacheCriticalSec);
		InitializeCriticalSection(&g_netCriticalSec);

		g_msgBuffer.Init();
		sessionCacheBuffer.Init();
		netMsgControl.Init(&g_msgBuffer,
			&g_netCriticalSec,
			&sessionCacheCriticalSec,
			&sessionCacheBuffer, &g_isStop);
		netMsgControl.start();

		while (m_Warrior1.IsAlive() && m_Warrior2.IsAlive())
		{
			if (g_isStop)
			{
				break;
			}

			UINT curTime = GetTickCount();
			static UINT lastTime = curTime;
			UINT timeElapse = curTime - lastTime;

			m_Warrior1.m_curCD -= timeElapse;
			m_Warrior2.m_curCD -= timeElapse;
			if (m_Warrior1.m_curCD < 0)
			{
				m_Warrior1.m_curCD = 0;
			}

			if (m_Warrior2.m_curCD < 0)
			{
				m_Warrior2.m_curCD = 0;
			}

			// ��ȡmsg
			ReturnStatusAsync ret = netMsgControl.FetchPacketList();

			DOLOG("test P2 CD:" + to_string(m_Warrior2.m_curCD) 
				+ ", timeLapse: " + to_string(timeElapse) + " \r\n");

			// Ai ����
			if (!m_Warrior2.IsInCD())
			{
				m_Warrior2.Attack(m_Warrior1);
				IUtil::SetINT(m_hwndP1Edit, m_Warrior1.m_curHP);
			}

			// ��Ҵ���ģ��
			for (auto pMsg : netMsgControl.m_ListMsg){
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

			// View����,��ǰ����ֻ����ʾһ����ҵ�CDʣ��
			IUtil::SetINT(m_hwndfightBtn, m_Warrior1.m_curCD);

			lastTime = curTime;
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

		if (g_isStop)
		{
			IUtil::SetText(m_hwndP2Edit, "��Ϸֹͣ");
			IUtil::SetText(m_hwndP1Edit, "��Ϸֹͣ");
		}
		return true;
	}
};