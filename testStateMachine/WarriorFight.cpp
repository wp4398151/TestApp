#include "stdafx.h"
#include "WarriorFight.h"
#include "IUtil.h"

CWarriorFight::CWarriorFight()
{
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

bool CWarriorFight::Init(HWND hwndP1Edit, HWND hwndP2Edit)
{
	m_hwndP1Edit = hwndP1Edit;
	m_hwndP2Edit = hwndP2Edit;

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
	IUtil::SeedRand();
	const int cdTime = 50;
	while (m_Warrior1.IsAlive() && m_Warrior2.IsAlive())
	{
		if (!m_Warrior1.IsInCD())
		{
			m_Warrior1.Attack(m_Warrior2);
			IUtil::SetINT(m_hwndP2Edit, m_Warrior2.m_curHP);
		}

		if (!m_Warrior2.IsInCD())
		{
			m_Warrior2.Attack(m_Warrior1);
			IUtil::SetINT(m_hwndP1Edit, m_Warrior1.m_curHP);
		}

		Sleep(cdTime);
		m_Warrior1.m_curCD -= cdTime;
		m_Warrior2.m_curCD  -= cdTime;
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
	return true;
}