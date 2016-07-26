#pragma once
#include <comip.h>

#define COMPTR(v) _com_ptr_t<_com_IIID<v,&__uuidof(v)>>
#ifdef _Debug
#define DEBUG(v)v
#define RELEASE(v)
#else
#define DEBUG(v)
#define RELEASE(v)v
#endif