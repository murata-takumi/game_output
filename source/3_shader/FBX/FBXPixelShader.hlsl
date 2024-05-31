#include"FBXShaderHeader.hlsli"

//FBXモデル用ピクセルシェーダー
//UV座標を基に頂点にテクスチャを付与する
float4 FBXPS(Output input) : SV_TARGET
{
	float4 ret = tex.Sample(smp,input.uv);

	float len = length(input.dis);
	float disFromEye = (len - 950.0f);
	
	ret.a = (disFromEye < 0.0f ? 1.0f : 1.0f - (disFromEye/ 50.0f));

	return ret;
}