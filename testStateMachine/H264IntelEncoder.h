#pragma once

class CH264IntelEncoder
{
public:
	CH264IntelEncoder();
	virtual ~CH264IntelEncoder();

	INT Init();
	INT Encode();
	UINT FetchOutput();
};

