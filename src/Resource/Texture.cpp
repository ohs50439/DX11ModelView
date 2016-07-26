#include <./Resource/Texture.h>
#include <./Device/DirectXDevice.h>

Texture2D::Texture2D() {
}
Texture2D::~Texture2D() {
}
ULONG Texture2D::Create(UINT Width, UINT Height, D3D11_USAGE use, DXGI_FORMAT fmt, UINT flg, UINT CPU_ACCESS_FLAG) {
	ULONG ret = 0;
	DX11Device &device = DX11Device::getInstance();
	ID3D11Texture2D *tex = nullptr;
	ID3D11ShaderResourceView *srv = nullptr;
	ID3D11RenderTargetView *rtv = nullptr;

	D3D11_TEXTURE2D_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.Width = Width;
	desc.Height = Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = fmt;
	desc.SampleDesc.Count = 1;
	desc.Usage = use;
	// シェーダリソースとして使う	
	desc.BindFlags = flg;
	// CPUからアクセスして書き込みOK
	desc.CPUAccessFlags = CPU_ACCESS_FLAG;
	//desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	if (FAILED(device.getDevice()->CreateTexture2D(&desc, nullptr, &tex))) {
		ret = -1;
		return ret;
	}
	this->tex.Attach(tex);

	// ShaderResourceViewの情報を作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;

	if (FAILED(device.getDevice()->CreateShaderResourceView(this->tex, &srvDesc, &srv))) {
		ret = -3;
		return ret;
	}
	this->srv.Attach(srv);

	if (flg&D3D11_BIND_RENDER_TARGET) {
		if (FAILED(device.getDevice()->CreateRenderTargetView(this->tex, nullptr, &rtv))) {
			ret = -2;
			return ret;
		}
		this->rtv.Attach(rtv);
	}
	return ret;
}
ULONG Texture2D::LoadFile(String str) {
	ULONG ret = 0;
	DX11Device &device = DX11Device::getInstance();
	ID3D11ShaderResourceView *srv = nullptr;
	if (FAILED(D3DX11CreateShaderResourceViewFromFile(device.getDevice(), str.c_str(), NULL, NULL, &srv, NULL))) {
		ret = -1;
		return ret;
	}
	this->srv.Attach(srv);
	return ret;
}