#include "stdafx.h"
#include "WaveFile.h"

CWaveFile::CWaveFile()
{
	m_pFile = NULL;
}

CWaveFile::~CWaveFile()
{
	WriteRIFFSize();
	WriteDATASize();

	if (m_pFile){
		fclose(m_pFile); 
		m_pFile = NULL;
	}
}

void CWaveFile::FillContent(WAVEFORMATEX& rFormat)
{
	m_waveContent.header.szRiffID[0] = 'R';
	m_waveContent.header.szRiffID[1] = 'I';
	m_waveContent.header.szRiffID[2] = 'F';
	m_waveContent.header.szRiffID[3] = 'F';

	m_waveContent.header.dwRiffSize = 0;

	m_waveContent.szRiffFormat[0] = 'W';
	m_waveContent.szRiffFormat[1] = 'A';
	m_waveContent.szRiffFormat[2] = 'V';
	m_waveContent.szRiffFormat[3] = 'E';

	m_waveContent.fmt.szFmtID[0] = 'f';
	m_waveContent.fmt.szFmtID[1] = 'm';
	m_waveContent.fmt.szFmtID[2] = 't';
	m_waveContent.fmt.szFmtID[3] = ' ';
	m_waveContent.fmt.dwFmtSize = sizeof(WAVEFORMATEX);
		
	m_waveContent.fmt.wavFormat.nAvgBytesPerSec = rFormat.nAvgBytesPerSec;
	m_waveContent.fmt.wavFormat.nBlockAlign = rFormat.nBlockAlign;
	m_waveContent.fmt.wavFormat.nChannels = rFormat.nChannels;
	m_waveContent.fmt.wavFormat.nSamplesPerSec = rFormat.nSamplesPerSec;
	//m_waveContent.fmt.wavFormat.wFormatTag = rFormat.wFormatTag;
	m_waveContent.fmt.wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	m_waveContent.fmt.wavFormat.wBitsPerSample = rFormat.wBitsPerSample;
	m_waveContent.fmt.wavFormat.cbSize = rFormat.cbSize;

	m_waveContent.dataHeader.szDataID[0] = 'd';
	m_waveContent.dataHeader.szDataID[1] = 'a';
	m_waveContent.dataHeader.szDataID[2] = 't';
	m_waveContent.dataHeader.szDataID[3] = 'a';

	m_waveContent.dataHeader.dwDataSize = 0;
}

bool CWaveFile::Init(string &strFile, WAVEFORMATEX& rFormat)
{
	m_pFile = fopen(strFile.c_str(), "w+b");
	if (!m_pFile)
	{
		LOG(ERROR) << "fopen failed errno:" << errno;
		return false;
	}
	m_strfilePath = strFile;
	FillContent(rFormat);

	size_t ret = fwrite(&m_waveContent, 1, sizeof(m_waveContent), m_pFile);
	if (sizeof(m_waveContent) != ret)
	{
		LOG(ERROR) << "fwrite wave header failed:" << errno;
		return false;
	}
	return true;
}

bool CWaveFile::WriteRIFFSize()
{
	long pos = ftell(m_pFile);
	
	if (fseek(m_pFile, 4, SEEK_SET))
	{
		LOG(ERROR) << "fseek(m_pFile failed:" << errno;
		return false;
	}
	m_waveContent.header.dwRiffSize = m_waveContent.dataHeader.dwDataSize + 32;
	size_t ret = fwrite(&m_waveContent.header.dwRiffSize, 1, 4, m_pFile);
	if (4 != ret)
	{
		LOG(ERROR) << "fwrite dwRiffSize failed errno:" << errno;
		return false;
	}

	if (fseek(m_pFile, pos, SEEK_SET))
	{
		LOG(ERROR) << "return file position failed errno:" << errno;
		return false;
	}

	return true;
}

bool CWaveFile::WriteDATASize()
{
	long pos = ftell(m_pFile);

	if (fseek(m_pFile, 42, SEEK_SET))
	{
		LOG(ERROR) << "fseek(m_pFile failed:" << errno;
		return false;
	}

	size_t ret = fwrite(&m_waveContent.dataHeader.dwDataSize, 1, 4, m_pFile);
	if (4 != ret)
	{
		LOG(ERROR) << "fwrite dwRiffSize failed errno:" << errno;
		return false;
	}

	if (fseek(m_pFile, pos, SEEK_SET))
	{
		LOG(ERROR) << "return file position failed errno:" << errno;
		return false;
	}

	return true;
}

bool CWaveFile::WriteSample(PBYTE pSample, UINT cbSize)
{
	size_t ret = fwrite(pSample, 1, cbSize, m_pFile);
	if (cbSize != ret)
	{
		LOG(ERROR) << "fwrite failed errno:" << errno;
		return false;
	}
	m_waveContent.dataHeader.dwDataSize += cbSize;
	return true;
}