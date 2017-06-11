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

// 收到此信息就会向AI发动攻击
struct FightMsg : public Msg{
	FightMsg (){
		id = EnumFightMsg;
	}
};

#pragma pack(pop)