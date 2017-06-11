#pragma once
#include "InitAttrs.h" 

namespace WUP{
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
		UINT m_ObjectID;		// �ö���Ϊȫ�ֶ����������ID��
							// ֻӦ���ڵ�ǰ�������н�����ѯ��
		INT m_curCD;
		INT m_curHP;

		INT m_hp;
		INT m_atkMin;
		INT m_atkMax;
		INT m_cdMin;
		INT m_cdMax;
	};

	class Player : public CWarrior
	{
	public:
		virtual ~Player();

	public:
		UINT m_userTokenID;		// ��ǰ�û���TokenID�����û���������ͨ��

		INT m_curCD;
		INT m_curHP;

		INT m_hp;
		INT m_atkMin;
		INT m_atkMax;
		INT m_cdMin;
		INT m_cdMax;
	};
};
