#pragma once

class GraphicStatus
{
public:
	GraphicStatus();
	~GraphicStatus();
	virtual bool SetStatus() = 0;
	virtual void AddRef()    = 0;
	virtual void Release()	 = 0;
private:


};
