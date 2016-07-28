#include <./Graphics/DX11/Rasterizer.h>
#include <./Device/DirectXDevice.h>


Rasterizer::Rasterizer() {

}
Rasterizer::~Rasterizer() {

}
bool Rasterizer::Create(D3D11_RASTERIZER_DESC *desc) {
	if (desc == nullptr || this->rasterizer) {
		return false;
	}
	DX11Device &device = DX11Device::getInstance();
	if (FAILED(device.getDevice()->CreateRasterizerState(desc, &this->rasterizer))) {
		return false;
	}
	return true;
}
bool Rasterizer::SetStatus() {
	if (this->rasterizer == nullptr)
		return false;
	DX11Device &device = DX11Device::getInstance();
	device.getContext()->RSSetState(this->rasterizer);
	return true;
}
void Rasterizer::AddRef() {
	if (this->rasterizer == nullptr)
		return;
	this->rasterizer.AddRef();
}
void Rasterizer::Release() {
	if (this->rasterizer == nullptr)
		return;
	this->rasterizer->Release();
	this->rasterizer = nullptr;
}