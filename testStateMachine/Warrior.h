#pragma once
#include "InitAttrs.h" 

class CWarrior
{
public:
	CWarrior();
	virtual ~CWarrior();
	bool Init(CInitAttrs* pAttrs);
	bool IsAlive();
	void Attack(CWarrior &rTargetWarrior);
	bool IsInCD();

public:
	INT m_curCD;
	INT m_curHP;

	INT m_hp;
	INT m_atkMin;
	INT m_atkMax;
	INT m_cdMin;
	INT m_cdMax;
};

