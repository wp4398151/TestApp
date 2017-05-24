#pragma once
#include <string>
#include <map>

using namespace std;

class CInitAttrs
{
public:
	CInitAttrs();
	~CInitAttrs();

	bool Init(string strFilePath);
	INT GetInt(LPSTR pStrAttrName);

	void ParseLine(char* pLine);
public:
	map<string, string> m_MapAttrs;
};

