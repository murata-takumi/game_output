#include"FBXShaderHeader.hlsli"

//FBXモデル用ピクセルシェーダー
//UV座標を基に頂点にテクスチャを付与する
float4 FBXPS(Output input) : SV_TARGET
{
	float4 ret = tex.Sample(smp,input.uv);

	float len = length(input.dis);
	
	ret.a = 0.0f;

	return ret;
}