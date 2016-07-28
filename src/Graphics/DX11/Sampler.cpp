#include <./Graphics/DX11/Sampler.h>
#include <./Device/DirectXDevice.h>

Sampler::Sampler(){
}

Sampler::~Sampler(){
}

bool Sampler::Attach(UINT pass, Sampler *ptr,UINT topass){
	if (ptr == nullptr || pass >= MAX_SAMPLER_PASS ||
	!ptr->sampler[pass] || ptr->sampler[topass]){
		return false;
	}
	this->sampler[pass] = ptr->sampler[topass];
	this->sampler[pass]->AddRef();
	return true;
}

bool Sampler::Create(UINT pass, D3D11_SAMPLER_DESC* desc){
	if (desc == nullptr || pass >= MAX_SAMPLER_PASS || this->sampler[pass]){
		return false;
	}
	DX11Device &device = DX11Device::getInstance();
	if (FAILED(device.getDevice()->CreateSamplerState(desc, &this->sampler[pass]))){
		return false;
	}
	return true;
}

bool Sampler::Remove(UINT pass){
	if (pass >= MAX_SAMPLER_PASS || !this->sampler[pass]){
		return false;
	}
	this->sampler[pass]->Release();
	this->sampler[pass] = nullptr;
	return true;
}

bool Sampler::SetStatus(){
	DX11Device &device = DX11Device::getInstance();
	for (int i = 0; i < MAX_SAMPLER_PASS; i++){
		ID3D11SamplerState *samp = this->sampler[i];
		device.getContext()->PSSetSamplers(i, 1, &samp);
	}
	return true;
}

void Sampler::AddRef(){
	for (auto &v : this->sampler){
		if (v){
			v->AddRef();
		}
	}
}

void Sampler::Release(){
	for (auto &v : this->sampler){

		if (v){
			v->Release();
			v = nullptr;
		}

	}
}