#include"FBXShaderHeader.hlsli"

//FBXモデル用頂点シェーダー
//パイプラインを通して受け取った頂点に対し各行列で座標変換を行う
Output FBXVS
(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	float3 tangent : TANGENT,
	float4 color : COLOR,
	uint4 ids : IDs,
	float4 weight : WEIGHT
)
{
	//単位行列
	const matrix identity =												
		float4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			);
	//ゼロ行列
	const matrix zeroMat =												
		float4x4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
			);

	//出力用構造体
	Output output;													

	//頂点に影響を与える4個のボーン行列をウェイトに応じて加算
	matrix boneTransform = bone[ids.r] * weight.r;					
	boneTransform += bone[ids.g] * weight.g;
	boneTransform += bone[ids.b] * weight.b;
	boneTransform += bone[ids.a] * weight.a;

	//ボーン行列が加算されていなかったら単位行列を代入
	boneTransform =													
		(boneTransform == zeroMat) ? identity : boneTransform;

	//座標変換
	//ボーン行列を反映
	float4 localPos = mul(boneTransform, float4(pos));				
	//ワールド行列を反映
	float4 worldPos = mul(world, localPos);							
	//ビュー行列を反映
	float4 viewPos = mul(view, worldPos);							
	//プロジェクション行列を反映
	float4 projPos = mul(proj, viewPos);
	
	//頂点座標
    output.pos = projPos;
	//システム座標
	output.svpos = projPos;	
    output.normal = normal;
	//カラー
	output.color = color;											
	//UV座標
	output.uv = uv;

	return output;
}