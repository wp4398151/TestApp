#pragma once
/**
* @file
* @brief Parse the stream and get packet, if it's invalid packet, it should be cached
*
* @author Wupeng <mailto:4398151@qq.com>
*
*/

#include "CacheBuffer.h"

#define TOKEN "wup"
// header
struct Packet{
	struct Header{
		unsigned char tok[4];	// tok = wup
		UINT cbBodySize;		// without header
	} header;
	LPBYTE pbody;				// body
};

class CPacketParser
{
public:
	CPacketParser(CCacheBuffer *pCacheBuffer) :m_pCacheBuffer(pCacheBuffer){ ; }
	virtual ~CPacketParser();
public:

	void ReleaseBuffer(LPBYTE pPacketBody);
	bool GetStreamBuffer(LPBYTE* ppPacketBody, int &cbRead);
	
private:
	CCacheBuffer *m_pCacheBuffer;
};

class CPacketWriter
{
public:
	CPacketWriter(CCacheBuffer *pCacheBuffer) :m_pCacheBuffer(pCacheBuffer){ ; }
	virtual ~CPacketWriter();
public:
	bool PutStreamBuffer(LPBYTE ppPacketBody, int cbSize);

private:
	CCacheBuffer *m_pCacheBuffer;
};


