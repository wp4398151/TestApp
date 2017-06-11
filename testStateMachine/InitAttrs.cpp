#include "stdafx.h"
#include "InitAttrs.h"
#include "IUtil.h"

#define DELIMITER '\n'

CInitAttrs::CInitAttrs()
{
	
}

CInitAttrs::~CInitAttrs()
{

}

// ����\nΪÿһ���еĽ�β
// ��β��Ҫ��һ��\n
bool CInitAttrs::ParseBuffer(WUP::CCacheBuffer* pBuffer)
{
	CHECK_NOTNULL(pBuffer);

	int lineCount = 0;
	BYTE lineBuffer[MAX_PATH + 1] = { 0 };
	while (pBuffer->GetSize())
	{
		if (lineCount>= MAX_PATH)
		{
			LOG(FATAL) << "������ÿһ�в��ܳ���MAX_PATH";
		}
		BYTE curChar = 0;
		int cbRead = 0;
		if(WUP::ReturnStatusAsync::Success == pBuffer->Read(&curChar, 1, cbRead))
		{
			if (curChar != DELIMITER)
			{
				lineBuffer[lineCount++] = curChar;
			}
			else
			{
				lineBuffer[lineCount] = '\0';
				ParseLine((char*)lineBuffer);
				lineCount = 0;
			}
		}
	}
	
	return true;
}

void CInitAttrs::ParseLine(char* pLine)
{
	CHAR* executableName = strchr(pLine, '=');
	executableName[0] = 0;
	m_MapAttrs.insert(make_pair(pLine, executableName + 1));
}

bool CInitAttrs::Init(string path)
{
	FILE *pFile = fopen(path.c_str(), "r");

	BYTE lineBuf[MAX_PATH] = {0};
	int isEOF = 0;
	WUP::CCacheBuffer buffer;
	buffer.Init();
	int cbRead = 0;
	while (isEOF == 0)
	{
		cbRead = fread(lineBuf, 1, MAX_PATH, pFile);
		isEOF = feof(pFile);
		if (cbRead == MAX_PATH || isEOF)
		{
			buffer.Write(lineBuf, cbRead);
		}
		else
		{
			LOG(FATAL) << "read file failed";
		}
	}

	fclose(pFile);
	// ��Ҫ����ȡ�������ݽ��з���
	//ParseLine(lineBuf);
	ParseBuffer(&buffer);
	buffer.Clear();


	return true;
}

//bool CInitAttrs::Init(string path)
//{
//	fstream fs;
//	fs.open(path, std::ios_base::in);
//	char lineBuf[MAX_PATH] = {0};
//	
//	if (fs.bad())
//	{
//		ALERT("bad!");
//	}
//
//	while (!fs.eof()){
//		fs.getline(lineBuf, MAX_PATH);
//		ParseLine(lineBuf);
//	}
//
//	return true;
//}

//bool CInitAttrs::Init(string path)
//{
//	fstream fs;
//	fs.open(path, std::ios_base::in);
//	char lineBuf[MAX_PATH] = {0};
//	
//	if (fs.bad())
//	{
//		ALERT("bad!");
//	}
//
//	while (!fs.eof()){
//		fs.getline(lineBuf, MAX_PATH);
//		ParseLine(lineBuf);
//	}
//
//	return true;
//}

INT CInitAttrs::GetInt(LPSTR pStrAttrName)
{
	map<string, string>::iterator itor = m_MapAttrs.find(string(pStrAttrName));
	if (itor == m_MapAttrs.end())
	{
		return 0;
	}
	return atoi(itor->second.c_str());
}