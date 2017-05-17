#pragma once
#include "IWaveFile.h"
#include <string>
#include <mmreg.h>

using namespace std;

#pragma pack(push, 1)

struct RIFF_HEADER
{
	char szRiffID[4];  // 'R','I','F','F'
	DWORD dwRiffSize;
};

//struct WAVE_FORMAT
//{
//	WORD wFormatTag;
//	WORD wChannels;
//	DWORD dwSamplesPerSec;
//	DWORD dwAvgBytesPerSec;
//	WORD wBlockAlign;
//	WORD wBitsPerSample;
//};

struct FMT_BLOCK
{
	char  szFmtID[4]; // 'f','m','t',' '
	DWORD  dwFmtSize; // sizeof(WAVEFORMAT)
	WAVEFORMATEX wavFormat;
};

struct DATA_BLOCK_HEADER
{
	char szDataID[4]; // 'd','a','t','a'
	DWORD dwDataSize;
};

struct WAVE_FILE_STRUCT
{
	RIFF_HEADER header;
	char szRiffFormat[4]; // 'W','A','V','E'
	FMT_BLOCK fmt;
	DATA_BLOCK_HEADER dataHeader;
};

class CWaveFile : public IWaveFile
{
public:
	CWaveFile();
	~CWaveFile();
	virtual bool WriteSample(PBYTE pSample, UINT cbSize);
	
	virtual bool WriteRIFFSize();
	virtual bool WriteDATASize();

	virtual bool Init(string &strFile, WAVEFORMATEX& rFormat);
	virtual void FillContent(WAVEFORMATEX& rFormat);

	virtual void ConvertPCM(PBYTE pSampleTarget, PBYTE pSampleSrc, UINT cbSize);

private:
	WAVE_FILE_STRUCT m_waveContent;

	FILE* m_pFile;
	string m_strfilePath;
};

#pragma pack(pop)
