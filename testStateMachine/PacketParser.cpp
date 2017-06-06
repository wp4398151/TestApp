#include "stdafx.h"
#include "PacketParser.h"
#include <assert.h>

CPacketParser::~CPacketParser()
{
	
}

void CPacketParser::ReleaseBuffer(PBYTE ppBuffer)
{
	SAFE_FREE(ppBuffer);
}

// return 如果成功获得一个packet就会输出一个packet, 并且返回true。失败返回false
bool CPacketParser::GetStreamBuffer(PBYTE* ppBuffer, int &cbRead)
{
	DCHECK_NOTNULL(m_pCacheBuffer);
	if (!m_pCacheBuffer){
		return false;
	}
	// check valid
	char validtoken[] = TOKEN;
	Packet::Header header;
	INT cbPeek= 0;

	if(!m_pCacheBuffer->Peek((LPBYTE)&header, sizeof(Packet::Header), cbPeek))
	{
		LOG(INFO) << "没有内容";
		return true;
	}
	if (cbPeek != sizeof(Packet::Header))
	{
		LOG(ERROR) << "内有足够内容";
		return true;
	}
	if (0 != strcmp((LPSTR)header.tok, TOKEN))
	{
		LOG(ERROR) << "内容不合法";
		return false;
	}

	if (m_pCacheBuffer->GetSize() < (header.cbBodySize + sizeof(Packet::Header)))
	{
		LOG(INFO) << "没有一个完整的Packet";
		return true;
	}
	
	*ppBuffer = (LPBYTE)malloc(header.cbBodySize);
	
	m_pCacheBuffer->Read((LPBYTE)&header, sizeof(Packet::Header), cbPeek);
	
	m_pCacheBuffer->Read(*ppBuffer, header.cbBodySize, cbRead);

	return true;
}

CPacketWriter::~CPacketWriter()
{

}

bool CPacketWriter::PutStreamBuffer(LPBYTE ppPacketBody, int cbSize)
{
	DCHECK_NOTNULL(m_pCacheBuffer);
	if (!m_pCacheBuffer){
		return false;
	}
	assert(m_pCacheBuffer->Write((LPBYTE)TOKEN, sizeof(TOKEN)));
	assert(m_pCacheBuffer->Write((LPBYTE)&cbSize, sizeof(int)));
	assert(m_pCacheBuffer->Write((LPBYTE)ppPacketBody, cbSize));

	return true;
}

