#include "stdafx.h"
#include "WarriorFight.h"
#include "IUtil.h"
// cfg.ini
#define P1HP "P1HP"
#define P1ATKMIN "P1ATKMIN"
#define P1ATKMAX "P1ATKMAX"
#define P1CDMIN "P1CDMIN"
#define P1CDMAX "P1CDMAX"

#define P2HP "P2HP"
#define P2ATKMIN "P2ATKMIN"
#define P2ATKMAX "P2ATKMAX"
#define P2CDMIN "P2CDMIN"
#define P2CDMAX "P2CDMAX"

CWarriorFight::CWarriorFight()
{
}

CWarriorFight::~CWarriorFight()
{
}

bool CWarriorFight::Init(CInitAttrs* pAttrs, HWND hwndP1Edit, HWND hwndP2Edit)
{
	if (!pAttrs)
	{
		LOG(ERROR) << "pAttrs should not be null";
		return false;
	}
	
	m_hwndP1Edit = hwndP1Edit;
	m_hwndP2Edit = hwndP2Edit;

	m_P1HP = pAttrs->GetInt(P1HP);
	m_P1ATKMIN = pAttrs->GetInt(P1ATKMIN);
	m_P1ATKMAX = pAttrs->GetInt(P1ATKMAX);
	m_P1CDMIN = pAttrs->GetInt(P1CDMIN);
	m_P1CDMAX = pAttrs->GetInt(P1CDMAX);

	m_P2HP = pAttrs->GetInt(P2HP);
	m_P2ATKMIN = pAttrs->GetInt(P2ATKMIN);
	m_P2ATKMAX = pAttrs->GetInt(P2ATKMAX);
	m_P2CDMIN = pAttrs->GetInt(P2CDMIN);
	m_P2CDMAX = pAttrs->GetInt(P2CDMAX);
	
	return true;
}

bool CWarriorFight::Fight()
{
	//string strHPP1, strHPP2;
	//IUtil::GetText(m_hwndP1Edit, strHPP1);
	//IUtil::GetText(m_hwndP2Edit, strHPP2);

	int hpP1 = m_P1HP;
	int hpP2 = m_P2HP;

	int cdP1 = IUtil::RangedRand(m_P1CDMIN, m_P1CDMAX);
	int cdP2 = IUtil::RangedRand(m_P2CDMIN, m_P2CDMAX);
	const int cdTime = 50;
	while (hpP1 > 0 && hpP2 > 0)
	{
		if (cdP1<0)
		{
			hpP2 -= IUtil::RangedRand(m_P1ATKMIN, m_P1ATKMAX);
			cdP1 = IUtil::RangedRand(m_P1CDMIN, m_P1CDMAX);
			IUtil::SetINT(m_hwndP2Edit, hpP2);
		}
		if (cdP2<0)
		{
			hpP1 -= IUtil::RangedRand(m_P2ATKMIN, m_P2ATKMAX);
			cdP2 = IUtil::RangedRand(m_P2CDMIN, m_P2CDMAX);
			IUtil::SetINT(m_hwndP1Edit, hpP1);
		}

		Sleep(cdTime);
		cdP1 -= cdTime;
		cdP2 -= cdTime;

		IUtil::RangedRand(0, 10);
	}


	if (hpP1 > 0)
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
