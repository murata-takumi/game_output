#include"FBXShaderHeader.hlsli"

//FBXモデル用ピクセルシェーダー
//UV座標を基に頂点にテクスチャを付与する
float4 FBXPS(Output input) : SV_TARGET
{
    float3 lightSrc = normalize(float3(1.0f, 1.0f, 0.0f));
    //float lightVec = input.svpos.xyz + lightSrc;
    float diffuseB = saturate(dot(input.pos.xyz, input.normal.xyz));
    
	{
        float p = dot(input.pos.xyz, lightSrc * -1.0f);
        p = (p * 0.5f) + 0.5f;
        p = p * p;
    }

	float4 ret = tex.Sample(smp,input.uv);
	
    //float4 toonDif = toonTex.Sample(toonSmp, float2(0.0f,p));
    float4 toonDif = toonTex.Sample(toonSmp, float2(0.0f,1.0f - diffuseB));
    
    ret.a = 1.0f;
    
	return toonDif * ret;
}