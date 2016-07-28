#pragma once
#pragma warning(disable:4005)
#include <d3d11_1.h>

class GraphicStatus {
public:
	virtual ~GraphicStatus();
	virtual bool SetStatus();
	virtual void AddRef();
	virtual void Release();
private:

};
