#pragma once

#include <list>

using namespace std;

#define CACHEBLOCKSIZE 8
class CCacheBuffer
{
public:
	CCacheBuffer();
	~CCacheBuffer();
	
public:
	bool Init();
	INT GetSize();
	bool Read(LPBYTE pBuf, INT cbBufferSize, INT &rcbRead);
	bool Peek(LPBYTE pBuf, INT cbBufferSize, INT &rcbPeek);
	bool Write(LPBYTE pBuf, INT cbBufferSize);
	void Clear();

	bool GetInfo(string& rstrInfo);

private:
	class CacheBlock
	{
	public:
		CacheBlock() : size(0), curPos(0)
		{
			memset(buf, 0, CACHEBLOCKSIZE);
		}

		void Clear(){ 
			size = 0; curPos = 0; 
#ifdef _DEBUG
			memset(buf, 0, CACHEBLOCKSIZE);
#endif
		}
		// return 是否需要读下一个Block
		bool Peek(LPBYTE pBuf, INT cbBufferSize, INT &cbRead)
		{
			char* pCurPtr = buf + curPos;
			if (size >= cbBufferSize){
				//直接读
				memcpy(pBuf, pCurPtr, cbBufferSize);
				cbRead = cbBufferSize;
				return false;
			}
			else{
				// 需要归还
				memcpy(pBuf, pCurPtr, size);
				cbRead = size;
				return true;
			}
		}

		// return 是否需要释放
		bool Read(LPBYTE pBuf, INT cbBufferSize, INT &cbRead)
		{
			char* pCurPtr = buf + curPos;
			if (size > cbBufferSize){
				//直接读
				memcpy(pBuf, pCurPtr, cbBufferSize);
				cbRead = cbBufferSize;
				curPos += cbRead;
				size -= cbRead;
				return false;
			}else{
				// 需要归还
				memcpy(pBuf, pCurPtr, size);
				cbRead = size;
				curPos = CACHEBLOCKSIZE;
				size = 0;
				return true;
			}
		}
		
		void GetInfo(string& rstrInfo){
			char strInfoContent[CACHEBLOCKSIZE+1] = {0};
			memcpy_s(strInfoContent, CACHEBLOCKSIZE + 1, buf, CACHEBLOCKSIZE);
			rstrInfo += "\r\n\t<<Block Info>> "
				"\r\n \t Pos: \t" + to_string(curPos) +
				"\r\n \t Size: \t" + to_string(size) +
				"\r\n \t Content: \t" + (LPCSTR)strInfoContent;
			
				return;
		}

		// return 是否需要重新申请
		bool Write(LPBYTE pBuf, INT cbBufferSize, INT &cbWrote)
		{
			USHORT curFreeSpace = CACHEBLOCKSIZE - (curPos + size);
			char* pCurPtr = buf + curPos + size;
			if (cbBufferSize < curFreeSpace){
				// 如果空间够就只写入需要的部分
				memcpy(pCurPtr, pBuf, cbBufferSize);
				cbWrote = cbBufferSize;
				size += cbBufferSize;
				return false;
			}else{
				// 空间不够就截断
				memcpy(pCurPtr, pBuf, curFreeSpace);
				cbWrote = curFreeSpace;
				size += curFreeSpace;
				return true;
			}
		}
		bool IsFull(){ ((curPos + size) >= CACHEBLOCKSIZE) ? true : false; }
		USHORT size;
		USHORT curPos;
		char buf[CACHEBLOCKSIZE];
	};

	CacheBlock* NewBlock(){ return new CacheBlock(); }
	void DelBlock(CacheBlock* pBlock){ delete pBlock; }

	CacheBlock* AllocBlock();
	void FreeBlock();

private:
	INT m_totalSize;

	typedef list<CacheBlock*> CacheBlockList;
	typedef list<CacheBlock*>::iterator CacheBlockListItor;
		
	CacheBlockList m_ListCacheBlock;
	CacheBlockList m_ListFreeBlock;
};

