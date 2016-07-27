struct VSOUT{
	float4 pos : SV_POSITION;
	//float4 color : COLOR0;
	//float4 normal : NORMAL;
	float2 uv  : TEXCOORD;
};
struct PSOUT {
	float4 albed   : SV_TARGET0;
	float4 normal  : SV_TARGET1;
	float  depth   : SV_TARGET2;
	float4 diffuse : SV_TARGET3;
};
SamplerState samLinear : register(s0); // どういうふうに大きくするか　(S)サンプラーが入ってるレジスター
Texture2D texAlbed	   : register(t0);  // (T)テクスチャが入ってるレジスター
Texture2D texAlbed2	   : register(t1);  // (T)テクスチャが入ってるレジスター


PSOUT main(VSOUT inp){
	PSOUT outp = (PSOUT)0;

	outp.albed = texAlbed.Sample(samLinear, inp.uv);
	outp.depth = inp.pos.z;
	//outp.diffuse = inp.color;
	outp.diffuse = texAlbed2.Sample(samLinear, inp.uv);
	//outp.normal = inp.normal;

	return outp;
}