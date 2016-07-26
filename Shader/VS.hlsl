cbuffer ConstantBuffer : register(b0){
	matrix World;
	matrix View;
	matrix Projection;
};
struct VSIN{
	float4 pos : POSITION;
};
struct VSOUT{
	float4 pos : SV_POSITION;
};
VSOUT main(VSIN inp){
	VSOUT outp = (VSOUT)0;
	outp.pos = mul(inp.pos, World);
	outp.pos = mul(outp.pos, View);
	outp.pos = mul(outp.pos,Projection);
	return outp;
}