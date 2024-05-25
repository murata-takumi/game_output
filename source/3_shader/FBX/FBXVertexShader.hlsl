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
	const matrix identity =												//単位行列
		float4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			);
	const matrix zeroMat =												//ゼロ行列
		float4x4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
			);

	Output output;													//出力用構造体

	matrix boneTransform = bone[ids.r] * weight.r;					//頂点に影響を与える4個のボーン行列をウェイトに応じて加算
	boneTransform += bone[ids.g] * weight.g;
	boneTransform += bone[ids.b] * weight.b;
	boneTransform += bone[ids.a] * weight.a;

	boneTransform =													//ボーン行列が加算されていなかったら単位行列を代入
		(boneTransform == zeroMat) ? identity : boneTransform;

	//座標変換
	float4 localPos = mul(boneTransform, float4(pos));				//ボーン行列を反映
	float4 worldPos = mul(world, localPos);							//ワールド行列を反映
	float4 viewPos = mul(view, worldPos);							//ビュー行列を反映
	float4 projPos = mul(proj, viewPos);							//プロジェクション行列を反映

	output.pos = viewPos;											//頂点座標
	output.svpos = projPos;											//頂点座標
	output.color = color;											//カラー
	output.uv = uv;													//UV座標

	return output;
}