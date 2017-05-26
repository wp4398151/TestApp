#include "stdafx.h"
#include "Warrior.h"
#include "IUtil.h"

#define HP "HP"
#define ATKMIN "ATKMIN"
#define ATKMAX "ATKMAX"
#define CDMIN "CDMIN"
#define CDMAX "CDMAX"

CWarrior::CWarrior()
{
	m_curCD = 0;
	m_curHP = 0;
}

CWarrior::~CWarrior()
{

}

bool CWarrior::Init(CInitAttrs* pAttrs)
{
	m_hp = pAttrs->GetInt(HP);
	m_atkMin = pAttrs->GetInt(ATKMIN);
	m_atkMax = pAttrs->GetInt(ATKMAX);
	m_cdMin = pAttrs->GetInt(CDMIN);
	m_cdMax = pAttrs->GetInt(CDMAX);

	m_curHP = m_hp;

	return true;
}

bool CWarrior::IsAlive()
{
	return (m_curHP > 0) ? true : false;
}

void CWarrior::Attack(CWarrior &rTargetWarrior)
{
	if (!rTargetWarrior.IsAlive()){
		return;
	}
	m_curCD = IUtil::RangedRand(m_cdMin, m_cdMax);
	
	INT attackVal = IUtil::RangedRand(m_atkMin, m_atkMax);
	rTargetWarrior.m_curHP -= attackVal;
	return;
}

bool CWarrior::IsInCD()
{
	return (m_curCD > 0) ? true : false;
}
