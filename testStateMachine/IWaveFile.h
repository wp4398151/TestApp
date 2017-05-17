#pragma once

class IWaveFile
{
public:
	virtual bool WriteSample(PBYTE pSample, UINT cbSize) = 0;
	virtual void ConvertPCM(PBYTE pSampleTarget, PBYTE pSampleSrc, UINT cbSize) = 0;
};

