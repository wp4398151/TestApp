#pragma once
#include "InitAttrs.h"
#include "Warrior.h"
#include "CacheBuffer.h"

namespace WUP{
	class CWarriorFight
	{
	public:
		CWarriorFight();
		virtual ~CWarriorFight();

		bool Init(HWND hwndP1Edit, HWND hwndP2Edit, HWND hwndfightBtn);
		bool GenerateCfg(CInitAttrs &initAttrInst, LPCSTR lpFileName);

		bool Fight();
		bool Start();

	public:
		HWND m_hwndP1Edit;
		HWND m_hwndP2Edit;
		HWND m_hwndfightBtn;

		CWarrior m_Warrior1;
		CWarrior m_Warrior2;

		WUP::CCacheBuffer* m_pMsgBuffer;	// Ä£ÄâÍøÂç½Ó¿Ú
	};
};
