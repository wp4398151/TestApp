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

	// return ����ɹ����һ��packet�ͻ����һ��packet, ���ҷ���true��ʧ�ܷ���false
	// \return	Success �ɹ���ȡ����
	//			Fatal ��Ҫֹͣ������,�����ǶϿ��û�����
	//			Skip ��һ���ٶ�ȡ��û�����ݱ�����
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
			LOG(INFO) << "û������";
			return Skip;
		}
		if (cbPeek != sizeof(Packet::Header))
		{
			LOG(ERROR) << "�����㹻����";
			return Skip;
		}
		if (0 != strcmp((LPSTR)header.tok, TOKEN))
		{
			LOG(ERROR) << "���ݲ��Ϸ�";
			return Fatal;
		}

		if (m_pCacheBuffer->GetSize() < (header.cbBodySize + sizeof(Packet::Header)))
		{
			LOG(INFO) << "û��һ��������Packet";
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