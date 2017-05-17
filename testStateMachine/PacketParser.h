#pragma once
/**
* @file
* @brief Parse the stream and get packet, if it's invalid packet, it should be cached
*
* @author Wupeng <mailto:4398151@qq.com>
*
*/


// One Packet Max Size Less than 1024
class CPacketParser
{
public:
	CPacketParser();
	virtual ~CPacketParser();

	bool GetStreamBuffer(, int &cbRead);
};

