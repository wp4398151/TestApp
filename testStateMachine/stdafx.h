// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


#include "glog/logging.h"
#include "glog/raw_logging.h"

#include <sstream>

#define DOLOG(s)	{						\
	std::stringstream ss;					\
	ss << s << std::endl;					\
	OutputDebugStringA(ss.str().c_str());	\
}

#define DOLOGW(s)	{						\
	std::wstringstream ss;					\
	ss << s << std::endl;					\
	OutputDebugStringW(ss.str().c_str());	\
}


#define SAFE_RELEASE(ptr) if(ptr){ ptr->Release(); ptr = NULL;}
#define SAFE_FREE(ptr) if(ptr){ free(ptr); ptr = NULL;}

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
