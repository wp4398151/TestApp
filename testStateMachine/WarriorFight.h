#pragma once
#include "InitAttrs.h"
#include "Warrior.h"

class CWarriorFight
{
public:
	CWarriorFight();
	virtual ~CWarriorFight();
	
	bool Init(HWND hwndP1Edit, HWND hwndP2Edit);
	bool GenerateCfg(CInitAttrs &initAttrInst, LPCSTR lpFileName);

	bool Fight();
public:
	HWND m_hwndP1Edit;
	HWND m_hwndP2Edit;
	
	CWarrior m_Warrior1;
	CWarrior m_Warrior2;
};

