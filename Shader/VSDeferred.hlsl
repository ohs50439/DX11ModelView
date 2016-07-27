cbuffer ConstantBuffer : register(b0){
	matrix World;
	matrix View;
	matrix Projection;
};
// VertexShaderInput(ì¸Åj
struct VSIN{
	float4 pos    : POSITION;
	//float4 color : COLOR0;
	//float4 normal : NORMAL;
	float2 uv     : TEXCOORD;
};
// VertexShaderOutput(èo)
struct VSOUT{
	float4 pos : SV_POSITION;
	//float4 color : COLOR0;
	//float4 normal : NORMAL;
	float2 uv  : TEXCOORD;
};
VSOUT main(VSIN inp){
	VSOUT outp = (VSOUT)0;
	outp.pos = mul(inp.pos, World);
	outp.pos = mul(outp.pos, View);
	outp.pos = mul(outp.pos,Projection);
	
	//outp.color = inp.color;

	//float3 normal;
	//normal = mul(inp.normal, (float3x3)World);
	//outp.normal = float4(normalize(normal),1.0f);

	outp.uv = inp.uv;
	return outp;
}