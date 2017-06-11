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


		// ģ��ͻ��˷���Fight��Ϣ
		bool Fight();
		
		bool Start();	// �������߼������

		void InitNet();	// ��ʼ������ģ��

		void DoHeartBeatObject();

	public:
		HWND m_hwndP1Edit;
		HWND m_hwndP2Edit;
		HWND m_hwndfightBtn;

		CWarrior m_Warrior1;
		CWarrior m_Warrior2;

		WUP::CCacheBuffer* m_pMsgBuffer;	// ģ������ӿ�


		// ������ģ��ͻ��˴��ڵ�
		CRITICAL_SECTION m_netCriticalSec;		// ģ��ͻ���������������߳�֮�䷢����Ϣ��ͬ������
		CCacheBuffer m_msgBuffer;				// ģ��ͻ��������������

		NetMsgControl m_netMsgControl;				// �����Լ�������ģ��
		CRITICAL_SECTION m_sessionCacheCriticalSec;	// �����̺߳ͷ������߼��߳�֮��ͬ������
		CCacheBuffer m_sessionCacheBuffer;			// �����̺߳��߼��س�֮��Ļ������ 

		//--------------------------------
		BOOL m_isStop = true;					// �������Ƿ��Ѿ�ֹͣ

		//-------------------------------- 
		ObjectMngr<CWarrior> m_WarriorMngr;		// �������е�Warrior����
	};
};
