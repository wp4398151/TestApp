#pragma once

class IWaveFile
{
public:
	virtual bool WriteSample(PBYTE pSample, UINT cbSize) = 0;
};

