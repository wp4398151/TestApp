#pragma once

#include "IUtil.h"
#include <list>
#include "PacketParser.h"
#include "ProtoDef.h"

namespace WUP{
	typedef std::list<Msg*> MsgList;
	typedef MsgList::iterator MsgListItor;

	class NetMsgControl : public LiteThread<NetMsgControl>
	{
	public:
		NetMsgControl() :LiteThread(this){
			m_pSessionCacheCriticalSec = NULL;
			m_pNetCriticalSec = NULL;
			m_pMsgBuffer = NULL;	// 模拟网络接口
			m_pIsStop = NULL;
			m_pSessionCacheBuffer = NULL;
			m_pPacketParser = NULL;
			ZeroMemory(m_MsgBuffer, MAX_PATH);
		}

		~NetMsgControl(){
			if (m_pPacketParser) delete m_pPacketParser;
		}

		//void WINAPI InitializeCriticalSection(
		//	__out  LPCRITICAL_SECTION lpCriticalSection
		//	);

		static unsigned __stdcall RunFunc(void* pThis)
		{
			CHECK_NOTNULL(pThis);
			NetMsgControl* pThisInst = (NetMsgControl*)pThis;

			ReturnStatusAsync ret = Success;
			bool isHaveData = true;
			while (TRUE != (*(pThisInst->m_pIsStop)))
			{
				isHaveData = true;
				// 读取网络过来的数据流
				while (isHaveData){
					int cbRead = 0;
					ret = pThisInst->GetBufferFake(pThisInst->m_MsgBuffer, MAX_PATH, cbRead);
					if (cbRead <= 0){
						isHaveData = false;
					}
					if (Success == ret)
					{
						EnterCriticalSection(pThisInst->m_pSessionCacheCriticalSec);

						ret = pThisInst->m_pSessionCacheBuffer->Write(pThisInst->m_MsgBuffer, cbRead);

						LeaveCriticalSection(pThisInst->m_pSessionCacheCriticalSec);

						if (Fatal == ret)
						{
							LOG(FATAL) << " pThisInst->m_pSessionCacheBuffer->Write Failed";
						}
					}
					else if (Fatal == ret)
					{
						LOG(FATAL) << " pThisInst->GetBufferFake Failed";
					}
				}
				Sleep(15);
			}
		}

		// 需要传入用于同步操作模拟网络接口类
		// 
		ReturnStatusAsync Init(CCacheBuffer* pMsgBuffer,
			CRITICAL_SECTION *pNetCriticalSec,
			CRITICAL_SECTION *pSessionCacheCriticalSec,
			CCacheBuffer* pSessionCacheBuffer,
			BOOL* pIsStop)
		{
			CHECK_NOTNULL(pMsgBuffer);
			CHECK_NOTNULL(pNetCriticalSec);
			CHECK_NOTNULL(pSessionCacheCriticalSec);
			CHECK_NOTNULL(pSessionCacheBuffer);

			if (!pMsgBuffer || !pNetCriticalSec)
			{
				return Fatal;
			}
			m_pMsgBuffer = pMsgBuffer;
			m_pNetCriticalSec = pNetCriticalSec;
			m_pSessionCacheCriticalSec = pSessionCacheCriticalSec;
			m_pSessionCacheBuffer = pSessionCacheBuffer;
			m_pIsStop = pIsStop;

			m_pPacketParser = new CPacketParser(pSessionCacheBuffer);
			if (!m_pPacketParser)
			{
				LOG(FATAL) << "new CPacketParser Failed";
				return Fatal;
			}
			return Success;
		}

		// 模拟从网络获得消息
		ReturnStatusAsync GetBufferFake(IN LPBYTE pBuffer, IN UINT cbBufferSize,
			OUT INT& cbRead)
		{
			ReturnStatusAsync ret = Skip;
			CHECK_NOTNULL(pBuffer);
			CHECK_NOTNULL(m_pNetCriticalSec);
			CHECK_NOTNULL(m_pMsgBuffer);

			EnterCriticalSection(m_pNetCriticalSec);
			if (m_pMsgBuffer->GetSize())
			{
				ret = m_pMsgBuffer->Read(pBuffer, cbBufferSize, cbRead);
			}
			LeaveCriticalSection(m_pNetCriticalSec);
			return ret;
		}

		// 将缓冲区中的数据转换成Msg存放在List中
		ReturnStatusAsync FetchPacketList()
		{
			CHECK_NOTNULL(m_pPacketParser);
			ReturnStatusAsync ret = Success;
			EnterCriticalSection(m_pSessionCacheCriticalSec);

			LPBYTE pPacketBody = NULL;
			int cbRead = 0;

			m_ListMsg.clear();

			while (ret == Success)
			{
				ret = m_pPacketParser->GetStreamBuffer(&pPacketBody, cbRead);
				if (ret == Success){
					m_ListMsg.push_back((Msg*)pPacketBody);
				}
			}

			LeaveCriticalSection(m_pSessionCacheCriticalSec);
			return ret;
		}

		BYTE m_MsgBuffer[MAX_PATH];

		CRITICAL_SECTION *m_pNetCriticalSec;
		CRITICAL_SECTION *m_pSessionCacheCriticalSec;
		CCacheBuffer* m_pMsgBuffer;	// 模拟网络接口
		CCacheBuffer* m_pSessionCacheBuffer;
		CPacketParser* m_pPacketParser;

		BOOL *m_pIsStop;

		MsgList m_ListMsg;
	public:

	};

};
