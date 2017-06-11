#include "stdafx.h"
#include "PacketParser.h"
#include <assert.h>

namespace WUP
{
	CPacketParser::~CPacketParser()
	{

	}

	void CPacketParser::ReleaseBuffer(PBYTE ppBuffer)
	{
		SAFE_FREE(ppBuffer);
	}

	// return 如果成功获得一个packet就会输出一个packet, 并且返回true。失败返回false
	// \return	Success 成功读取到包
	//			Fatal 需要停止当程序,或者是断开该缓冲区
	//			Skip 下一次再读取，没有数据被读出
	ReturnStatusAsync CPacketParser::GetStreamBuffer(PBYTE* ppBuffer, int &cbRead)
	{
		DCHECK_NOTNULL(m_pCacheBuffer);
		if (!m_pCacheBuffer){
			return Fatal;
		}
		// check valid
		char validtoken[] = TOKEN;
		Packet::Header header;
		INT cbPeek = 0;

		if (Success != m_pCacheBuffer->Peek((LPBYTE)&header, sizeof(Packet::Header), cbPeek))
		{
			LOG(INFO) << "没有内容";
			return Skip;
		}
		if (cbPeek != sizeof(Packet::Header))
		{
			LOG(ERROR) << "内有足够内容";
			return Skip;
		}
		if (0 != strcmp((LPSTR)header.tok, TOKEN))
		{
			LOG(ERROR) << "内容不合法";
			return Fatal;
		}

		if (m_pCacheBuffer->GetSize() < (header.cbBodySize + sizeof(Packet::Header)))
		{
			LOG(INFO) << "没有一个完整的Packet";
			return Skip;
		}

		*ppBuffer = (LPBYTE)malloc(header.cbBodySize);

		m_pCacheBuffer->Read((LPBYTE)&header, sizeof(Packet::Header), cbPeek);
		m_pCacheBuffer->Read(*ppBuffer, header.cbBodySize, cbRead);
		return Success;
	}

	CPacketWriter::~CPacketWriter()
	{

	}

	ReturnStatusAsync CPacketWriter::PutStreamBuffer(LPBYTE ppPacketBody, int cbSize)
	{
		DCHECK_NOTNULL(m_pCacheBuffer);
		if (!m_pCacheBuffer){
			return Fatal;
		}
		assert(Success == m_pCacheBuffer->Write((LPBYTE)TOKEN, sizeof(TOKEN)));
		assert(Success == m_pCacheBuffer->Write((LPBYTE)&cbSize, sizeof(int)));
		assert(Success == m_pCacheBuffer->Write((LPBYTE)ppPacketBody, cbSize));
		return Success;
	}


};