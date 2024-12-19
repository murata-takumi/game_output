#include"FBXShaderHeader.hlsli"

//FBXモデル用ピクセルシェーダー
//UV座標を基に頂点にテクスチャを付与する
float4 FBXPS(Output input) : SV_TARGET
{
    float3 inversedLightl = mul(inverse(world), light);
    float diffuseB = saturate(dot(inversedLightl, input.normal.xyz));

	float4 ret = tex.Sample(smp,input.uv);
	
    float4 toonDif = toonTex.Sample(toonSmp, float2(0.0f,1.0f - diffuseB));
    
    float dis = length(input.pos.xyz);
    dis -= 1500.0f;
    
    ret.a = (dis < 0.0f ? 1.0f : 1.0f - (dis / 50.0f));
    
	return toonDif * ret;
}