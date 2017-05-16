#pragma once
#include <string>
#include <vector>

#include <Mmdeviceapi.h>
#include <Audioclient.h>
#include <propsys.h>
#include <Functiondiscoverykeys_devpkey.h>
#include "IWaveFile.h"

using namespace std;

struct AudioDeviceInfo
{
wstring strID;
wstring strName;
};

//1000 microseconds, then 10 "100nanosecond" segments
#define CONVERTMSTO100NANOSEC(ms) (ms * 1000 * 10) 

class CaptureAudioClass
{
public:
	CaptureAudioClass(IWaveFile* pFile);
	virtual ~CaptureAudioClass();
	virtual bool InitFormat();
	virtual bool InitDevice(
					EDataFlow deviceType, // eCapture, eRender, eAll
					DWORD deviceFlag); //DEVICE_STATE_ACTIVE, DEVICE_STATE_UNPLUGGED;
	virtual void GetDeviceName(wstring& rDeviceName) const
	{
		rDeviceName = m_VectorAudioList[m_indexSelectAudioDeviceID].strName;
	}

	virtual bool GetNextBuffer(UINT& numFrames);
	virtual bool DoRecordLoop();
	virtual void Start();
	virtual void Stop();

	IWaveFile*				m_pFile;
	UINT					m_indexSelectAudioDeviceID;
	vector<AudioDeviceInfo>	m_VectorAudioList;
	WAVEFORMATEX*			m_pWaveFormatEx;
	IMMDevice*				m_mmDevice;
	IAudioClient*			m_mmClient;
	IAudioCaptureClient*	m_mmCapture;
	IAudioClock*			m_mmClock;
	UINT					m_bufferFrameCount;

	vector<PBYTE>			m_pAudioContentVector;
};