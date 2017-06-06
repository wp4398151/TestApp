// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
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

// TODO:  在此处引用程序需要的其他头文件
