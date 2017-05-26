#include "stdafx.h"
#include "InitAttrs.h"
#include <fstream>

CInitAttrs::CInitAttrs()
{
	
}


CInitAttrs::~CInitAttrs()
{

}

void CInitAttrs::ParseLine(char* pLine)
{
	CHAR* executableName = strchr(pLine, '=');
	executableName[0] = 0;
	m_MapAttrs.insert(make_pair(pLine, executableName + 1));
}

bool CInitAttrs::Init(string path)
{
	fstream fs(path, ios_base::in);
	char lineBuf[MAX_PATH] = {0};

	while (!fs.eof()){
		fs.getline(lineBuf, MAX_PATH);
		ParseLine(lineBuf);
	}

	return true;
}

INT CInitAttrs::GetInt(LPSTR pStrAttrName)
{
	map<string, string>::iterator itor = m_MapAttrs.find(string(pStrAttrName));
	if (itor == m_MapAttrs.end())
	{
		return 0;
	}
	return atoi(itor->second.c_str());
}