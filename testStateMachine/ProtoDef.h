#pragma once

#pragma pack(push, 1)

enum ProtoMsg{
	EnumTestMsg = 0,
	EnumFightMsg = 1,
};

struct Msg
{
	unsigned short id = EnumTestMsg;
};

struct TestMsg : public Msg{
	TestMsg(){ 
		id = EnumTestMsg;
	}
};

// �յ�����Ϣ�ͻ���AI��������
struct FightMsg : public Msg{
	FightMsg (){
		id = EnumFightMsg;
	}
};

#pragma pack(pop)