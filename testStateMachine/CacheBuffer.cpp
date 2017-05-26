#include "stdafx.h"
#include "CacheBuffer.h"

CCacheBuffer::CCacheBuffer() : m_totalSize(0)
{

}

CCacheBuffer::~CCacheBuffer()
{

}

bool CCacheBuffer::Init()
{
	Clear();
	CacheBlock* pCurBlock = AllocBlock();
	if (pCurBlock == NULL){
		LOG(ERROR) << "New Block ʧ��!";
		return false;
	}
	return true;
}

bool CCacheBuffer::GetInfo(string& rstrInfo)
{
	rstrInfo += "\r\n<<CCacheBuffer Info>> "
		"\r\n size: \t" + to_string(m_totalSize) +
		"\r\n Cache Block: \t" + to_string(m_ListCacheBlock.size()) +
		"\r\n Free Block: \t" + to_string(m_ListFreeBlock.size());
	rstrInfo += "\r\n\t----- Cache Block Detail";
	for each (CacheBlock* var in m_ListCacheBlock)
	{
		var->GetInfo(rstrInfo);
	}
	rstrInfo += "\r\n\t----- Free Block Detail";
	for each (CacheBlock* var in m_ListFreeBlock)
	{
		var->GetInfo(rstrInfo);
	}
	return true;
}

bool CCacheBuffer::Write(LPBYTE pBuf, INT cbBufferSize)
{
	if (cbBufferSize == 0 || pBuf == NULL)
	{
		LOG(ERROR) << "��ȡ���Ȼ����ݲ��Ϸ�";
		return false;
	}

	// ������ָպ�д��ָ������Ч�������ÿ��д���ͣ������Ч��λ��
	INT sizeRemain = cbBufferSize;
	INT sizeWrote = 0;
	INT curPos = 0;
	while (sizeRemain > 0)
	{
		CacheBlock* pEndBlock = m_ListCacheBlock.back();
		sizeWrote = 0;
		bool bNeedAlloc = pEndBlock->Write(pBuf + curPos, sizeRemain, sizeWrote);
		sizeRemain -= sizeWrote;
		if (bNeedAlloc)
		{
			AllocBlock();
		}
		curPos += sizeWrote;
	}
	m_totalSize += cbBufferSize;

	return true;
}

bool CCacheBuffer::Peek(LPBYTE pBuf, INT cbBufferSize, INT &rcbPeek)
{
	if (cbBufferSize == 0 || pBuf == NULL)
	{
		LOG(ERROR) << "��ȡ���Ȼ����ݲ��Ϸ�";
		return false;
	}

	INT sizeRemain = (cbBufferSize < m_totalSize) ? cbBufferSize : m_totalSize;
	INT sizePeek = 0;
	INT curPos = 0;
	rcbPeek = sizeRemain;
	CacheBlockListItor itor = m_ListCacheBlock.begin();
	while (sizeRemain > 0)
	{
		CacheBlock* pCurBlock = *itor;
		sizePeek = 0;
		bool bNeedMoveNextBlock = pCurBlock->Peek(pBuf + curPos, sizeRemain, sizePeek);
		if (bNeedMoveNextBlock){
			++itor;
		}
		sizeRemain -= sizePeek;
		curPos += sizePeek;
	}
	return true;
}

bool CCacheBuffer::Read(LPBYTE pBuf, INT cbBufferSize, INT &rcbRead)
{
	if (cbBufferSize == 0 || pBuf == NULL)
	{
		LOG(ERROR) << "��ȡ���Ȼ����ݲ��Ϸ�";
		return false;
	}

	INT sizeRemain = (cbBufferSize < m_totalSize) ? cbBufferSize : m_totalSize;
	INT sizeRead = 0;
	INT curPos = 0;

	m_totalSize -= sizeRemain;
	rcbRead = sizeRemain;
	CacheBlockListItor itor = m_ListCacheBlock.begin();
	while (sizeRemain > 0)
	{
		CacheBlock* pCurBlock = *itor;
		sizeRead = 0;
		bool bNeedMoveNextBlock = pCurBlock->Read(pBuf + curPos, sizeRemain, sizeRead);
		if (bNeedMoveNextBlock){
			// ������, ���һ��ʱ,ֱ����յ�ǰ��
			if ((++itor) == m_ListCacheBlock.end()){
				(*m_ListCacheBlock.begin())->Clear();
			}
			else
			{
				FreeBlock();
			}
			
			// ����һ�������һ��ʱ���·���
			//if ((++itor) == m_ListCacheBlock.end()){
			//	// �����ǰ�����һ�飬��Ҫ���·���
			//	CacheBlock* pBlock = AllocBlock();
			//	if (pBlock == NULL){
			//		LOG(ERROR) << "�����ʧ��";
			//		return false;
			//	}
			//}
			//FreeBlock();
		}
		sizeRemain -= sizeRead;
		curPos += sizeRead;
	}

	return true;
}

CCacheBuffer::CacheBlock* CCacheBuffer::AllocBlock()
{
	CacheBlock* pBlock = NULL;
	CacheBlockListItor itor = m_ListFreeBlock.begin();
	if (itor == m_ListFreeBlock.end()){
		pBlock = NewBlock();
	}
	else
	{
		pBlock = (*itor);
		m_ListFreeBlock.pop_front();
	}
	if (pBlock == NULL){
		LOG(ERROR) << "new �µ�Blockʧ��";
		return NULL;
	}

	m_ListCacheBlock.push_back(pBlock);
	return pBlock;
}

void CCacheBuffer::FreeBlock()
{
	CacheBlockListItor itor = m_ListCacheBlock.begin();
	if (itor == m_ListCacheBlock.end())
	{
		return ;
	}
	(*itor)->Clear();
	m_ListFreeBlock.push_front(*itor);
	m_ListCacheBlock.pop_front();
	return ;
}

INT CCacheBuffer::GetSize()
{
	return m_totalSize;
}

void CCacheBuffer::Clear()
{
	m_totalSize = 0;
	for each (CacheBlock* pBlock in m_ListCacheBlock){
		DelBlock(pBlock);
	}
	m_ListCacheBlock.clear();
	for each (CacheBlock* pBlock in m_ListFreeBlock){
		DelBlock(pBlock);
	}
	m_ListFreeBlock.clear();
}