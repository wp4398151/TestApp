#include "stdafx.h"
#include "CaptureAudio.h"

// refer to https://msdn.microsoft.com/en-us/library/windows/desktop/dd370800(v=vs.85).aspx

CaptureAudioClass::CaptureAudioClass(IWaveFile* pFile)
{
	m_pFile = pFile;
	m_pWaveFormatEx = NULL;
	m_indexSelectAudioDeviceID = 1;
	m_mmDevice = NULL;
	m_mmClient = NULL;
	m_mmCapture = NULL;
	m_mmClock = NULL;
	m_bufferFrameCount = 0;
}

CaptureAudioClass::~CaptureAudioClass()
{
	if (m_pWaveFormatEx)CoTaskMemFree(m_pWaveFormatEx);
	SAFE_RELEASE(m_mmDevice);
	SAFE_RELEASE(m_mmClient);
	SAFE_RELEASE(m_mmCapture);
	SAFE_RELEASE(m_mmClock);
}

void CaptureAudioClass::Start()
{
	if (m_mmClient)
	{
		m_mmClient->Start();

		UINT64 freq;
		m_mmClock->GetFrequency(&freq);

		//Log(TEXT("MMDeviceAudioSource: Frequency for device '%s' is %llu, samples per sec is %u"), GetDeviceName(), freq, this->GetSamplesPerSec());
	}
}

void CaptureAudioClass::Stop()
{
	if (m_mmClient)
	{
		m_mmClient->Stop();
	}
}

bool CaptureAudioClass::InitFormat()
{
	HRESULT err = S_OK;
	err = m_mmClient->GetMixFormat(&m_pWaveFormatEx);
	if (FAILED(err))
	{
		LOG(ERROR) << "m_mmClient->GetMixFormat Failed " << err;
		return false;
	}

	err = m_mmClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 
							AUDCLNT_STREAMFLAGS_LOOPBACK,
								CONVERTMSTO100NANOSEC(5000),  
								0, m_pWaveFormatEx, NULL);
	if (FAILED(err))
	{
		LOG(ERROR) << "m_mmClient->Initialize faled " << err;
		return false;
	}

	// Get the size of the allocated buffer.
	err = m_mmClient->GetBufferSize(&m_bufferFrameCount);
	
	DOLOG("DebugAudio: GetBufferSize:"<<m_bufferFrameCount);

	err = m_mmClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_mmCapture);
	if (FAILED(err))
	{
		return false;
	}

	err = m_mmClient->GetService(__uuidof(IAudioClock), (void**)&m_mmClock);
	if (FAILED(err))
	{
		return false;
	}

	return true;
}

bool CaptureAudioClass::InitDevice(EDataFlow deviceType, DWORD deviceFlag)
{
	LOG(INFO) << "Init Device ...";

	const CLSID CLSID_MMDeviceEnumerator1 = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator1 = __uuidof(IMMDeviceEnumerator);
	IMMDeviceEnumerator *mmEnumerator = NULL;
	HRESULT err = S_OK;

	err = CoCreateInstance(CLSID_MMDeviceEnumerator1, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator1, (void**)&mmEnumerator);
	if (FAILED(err))
	{
		LOG(ERROR) << "GetAudioDevices: Could not create IMMDeviceEnumerator: " << err;
		return false;
	}
	IMMDeviceCollection *collection = NULL;
	err = mmEnumerator->EnumAudioEndpoints(deviceType, deviceFlag, &collection);
	if (FAILED(err))
	{
		LOG(ERROR) << ("GetAudioDevices: Could not enumerate audio endpoints");
		SAFE_RELEASE(mmEnumerator)
		return false;
	}

	UINT count = 0;
	if (SUCCEEDED(collection->GetCount(&count)))
	{
		for (UINT i = 0; i < count; i++)
		{
			IMMDevice *device = NULL;
			if (SUCCEEDED(collection->Item(i, &device)))
			{
				LPCWSTR wstrID = NULL;
				if (SUCCEEDED(device->GetId((LPWSTR*)&wstrID)))
				{
					IPropertyStore *store = NULL;
					if (SUCCEEDED(device->OpenPropertyStore(STGM_READ, &store)))
					{
						PROPVARIANT varName;
						PropVariantInit(&varName);
						if (SUCCEEDED(store->GetValue(PKEY_Device_FriendlyName, &varName)))
						{
							LPCWSTR wstrName = varName.pwszVal;
							AudioDeviceInfo audioInfo = { wstrID, wstrName };
							audioInfo.strID = wstrID;
							audioInfo.strName = wstrName;


							m_VectorAudioList.push_back(audioInfo);
						}
					}
					CoTaskMemFree((LPVOID)wstrID);
				}
				SAFE_RELEASE(device);
			}
		}
	}

	LPCWSTR wzDeviceName = m_VectorAudioList[m_indexSelectAudioDeviceID].strName.c_str();
	DOLOGW(L"DebugAudio: Device Name:" << wzDeviceName );

	err = mmEnumerator->GetDevice(m_VectorAudioList[m_indexSelectAudioDeviceID].strID.c_str() , &m_mmDevice);

	LOG(INFO) << "mmEnumerator->GetDevice "<<err;
	if (FAILED(err))
	{
		LOG(ERROR) << "mmEnumerator->GetDevice "<<err;
		return false;
	}
	
	err = m_mmDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&m_mmClient);
	if (FAILED(err))
	{
		LOG(ERROR) << "m_mmDevice->Activate " << err;
		return false;
	}

	SAFE_RELEASE(collection);
	SAFE_RELEASE(mmEnumerator);
	return true;
}

bool CaptureAudioClass::DoRecordLoop()
{
	UINT dwSleepTimeMS = (m_bufferFrameCount / m_pWaveFormatEx->nSamplesPerSec) * 1000;

	UINT curTime = 0;
	UINT numFrames = 0;
	UINT nextTime = 0;
	nextTime = GetTickCount() + dwSleepTimeMS;
	for (int i = 0; i < 3; ++i)
	{
		curTime = GetTickCount();
		DOLOG("Sleep: cur:" << curTime << ", nextTime:" << nextTime);

		if ((nextTime - curTime) > 0)
		{
			DOLOG("Sleep: nextTime - curTime = "<<(nextTime - curTime));
			Sleep(nextTime - curTime);
		}
		nextTime = GetTickCount() + dwSleepTimeMS;

		if (!GetNextBuffer(numFrames))
		{
			LOG(ERROR) << "GetNextBuffer(&numFrames) Failed ";
			return false;
		}
	}
	return true;
}

bool CaptureAudioClass::GetNextBuffer(UINT &numFrames)
{
	HRESULT hRes = S_OK;
	UINT packetLength = 0;
	hRes = m_mmCapture->GetNextPacketSize(&packetLength);
	PBYTE pData = NULL;
	PBYTE captureBuffer = NULL;

	while (packetLength != 0)
	{
		UINT64 devicePosition = 0;
		UINT64 qpcPosition = 0;
		UINT numFramesAvailable = 0;
		DWORD flags = 0;
		// Get the available data in the shared buffer.
		hRes = m_mmCapture->GetBuffer(&pData, &numFramesAvailable, &flags, &devicePosition, &qpcPosition);

		if (!SUCCEEDED(hRes))
		{
			LOG(ERROR) << "m_mmCapture->GetBuffer Failed (" << hRes << ")";
			return false;
		}

		DOLOG("DebugAudio: numFramesAvailable:"<<numFrames<<", devicePosition:"<<devicePosition<<", qpcPosition:"<<qpcPosition);

		if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
		{
			//pData = NULL;  // Tell CopyData to write silence.
		}

		INT bufferSize = numFramesAvailable*m_pWaveFormatEx->wBitsPerSample / 8;

		if (m_pFile && !m_pFile->WriteSample(pData, bufferSize))
		{
			LOG(ERROR) << "m_pFile->Write Failed " ;
			return false;
		}

		// Copy the available capture data to the audio sink.
		hRes = m_mmCapture->ReleaseBuffer(numFramesAvailable);
		if (!SUCCEEDED(hRes))
		{
			LOG(ERROR) << "m_mmCapture->ReleaseBuffer Failed ("<<hRes<< ")";
			return false;
		}

		hRes = m_mmCapture->GetNextPacketSize(&packetLength);
		if (!SUCCEEDED(hRes))
		{
			LOG(ERROR) << "m_mmCapture->ReleaseBuffer Failed ("<<hRes<< ")";
			return false;
		}
		DOLOG("DebugAudio: NextPacketSize:" << packetLength);
	}
	DOLOG("DebugAudio: packetLength = 0--------------------------------------------------");
	return true;
}