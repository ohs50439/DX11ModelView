#pragma once
#include <d3d11_1.h>
#include <Macro.h>

class Texture2D  {
public:
	Texture2D();
	~Texture2D();

	ULONG Create(UINT, UINT, D3D11_USAGE, DXGI_FORMAT, UINT = D3D11_BIND_SHADER_RESOURCE, UINT CPU_ACCESS_FLAG = 0);
	ULONG LoadFile(String file);

	ID3D11Texture2D				*getTex() { return tex; }
	ID3D11ShaderResourceView	*getSRV() { return srv; }
	ID3D11RenderTargetView		*getRTV() { return rtv; }
protected:
private:
	COMPTR(ID3D11Texture2D)				tex;
	COMPTR(ID3D11ShaderResourceView)	srv;
	COMPTR(ID3D11RenderTargetView)		rtv;
};