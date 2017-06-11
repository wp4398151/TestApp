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
		UINT m_ObjectID;		// 该对象为全局对象管理器中ID。
							// 只应用于当前服务器中交互查询。
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
		UINT m_userTokenID;		// 当前用户的TokenID，在用户管理器中通过

		INT m_curCD;
		INT m_curHP;

		INT m_hp;
		INT m_atkMin;
		INT m_atkMax;
		INT m_cdMin;
		INT m_cdMax;
	};
};
