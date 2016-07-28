#pragma once
#pragma warning(disable:4005)
#include <d3d11_1.h>

class GraphicStatus
{
public:
	GraphicStatus();
	~GraphicStatus();
	virtual bool SetStatus()	= 0;
	virtual void AddRef()		= 0;
	virtual void Release()		= 0;
private:

};
