#pragma once
#include <vector>
#include <./Graphics/GraphicStatus.h>
#include <./Graphics/GraphicObject.h>

class GraphicsPipeLine{
public:
	GraphicsPipeLine();
	GraphicsPipeLine(unsigned long size);
	~GraphicsPipeLine();
	void setStatus();
	virtual bool Draw()						= 0;
	virtual bool Attach(GraphicStatus* ptr) = 0;
	virtual bool Remove(GraphicStatus* ptr) = 0;
	void Release();
protected:

private:
	std::vector<GraphicStatus*> status;
	GraphicObject *object;
};
