#pragma once
#include <string>
#include <map>
#include "CacheBuffer.h"

using namespace std;

class CInitAttrs
{
public:
	CInitAttrs();
	~CInitAttrs();

	bool Init(string strFilePath);
	INT GetInt(LPSTR pStrAttrName);

	void ParseLine(char* pLine);
	bool ParseBuffer(WUP::CCacheBuffer* pBuffer);
public:
	map<string, string> m_MapAttrs;
};

