#pragma once
#include <comip.h>
#include <string>
#ifdef _UNICODE
typedef std::wstring String;
typedef LPCWSTR LPCGFWCHAR;
typedef WCHAR GFWCHAR;
#else
typedef std::string String;
typedef LPCSTR LPCGFWCHAR;
typedef CHAR GFWCHAR;
#endif

#define SAFE_RELEASE(v) if(v) {v->Release(); v = nullptr;}
#define COMPTR(v) _com_ptr_t<_com_IIID<v,&__uuidof(v)>>
#ifdef _Debug
#define DEBUG(v)v
#define RELEASE(v)
#else
#define DEBUG(v)
#define RELEASE(v)v
#endif