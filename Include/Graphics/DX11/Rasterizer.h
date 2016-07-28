#pragma once
#include <./Macro.h>
#include <./Graphics/GraphicStatus.h>

class Rasterizer : GraphicStatus {
public:
	Rasterizer();
	~Rasterizer();
	bool Create(D3D11_RASTERIZER_DESC *desc);
	bool SetStatus();
	void AddRef();
	void Release();

private:
	COMPTR(ID3D11RasterizerState) rasterizer;
};
