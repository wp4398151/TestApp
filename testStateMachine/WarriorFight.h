#pragma once
#include "InitAttrs.h"

class CWarriorFight
{
public:
	CWarriorFight();
	virtual ~CWarriorFight();
	
	bool Init(CInitAttrs* pAttrs, HWND hwndP1Edit, HWND hwndP2Edit);

	bool Fight();
public:
	HWND m_hwndP1Edit;
	HWND m_hwndP2Edit;

	INT m_P1HP;
	INT m_P1ATKMIN;
	INT m_P1ATKMAX;
	INT m_P1CDMIN;
	INT m_P1CDMAX;

	INT m_P2HP;
	INT m_P2ATKMIN;
	INT m_P2ATKMAX;
	INT m_P2CDMIN;
	INT m_P2CDMAX;
};

