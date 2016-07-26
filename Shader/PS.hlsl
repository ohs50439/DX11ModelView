struct VSOUT{
	float4 pos : SV_POSITION;
};
float4 main(VSOUT inp) : SV_TARGET{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}