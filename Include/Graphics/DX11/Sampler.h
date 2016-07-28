#pragma once
#include <./Macro.h>
#include <./Graphics/GraphicStatus.h>

#define MAX_SAMPLER_PASS 6

class Sampler : public GraphicStatus
{
public:
	Sampler();
	~Sampler();

	bool Attach(UINT pass,Sampler *ptr,UINT topass);
	bool Create(UINT pass,D3D11_SAMPLER_DESC* desc);
	bool SetStatus();
	bool Remove(UINT pass);
	void AddRef();
	void Release();
	

private:
	COMPTR(ID3D11SamplerState) sampler[MAX_SAMPLER_PASS];

};


