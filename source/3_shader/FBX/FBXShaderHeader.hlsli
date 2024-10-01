//FBXモデル用ヘッダーファイル
//シェーダーで用いる各種データをまとめている

//テクスチャ
Texture2D<float4> tex : register(t0);	
//トゥーンテクスチャ
Texture2D<float4> toonTex : register(t1);	

//サンプラー（テクスチャの取得方法を決める）
SamplerState smp : register(s0);	
//サンプラー（トゥーンテクスチャの取得方法を決める）
SamplerState toonSmp : register(s1);

//ビュープロジェクション用スロット
cbuffer SceneData : register(b0)
{
	//ビュー行列
	matrix view;						
	//プロジェクション行列
	matrix proj;						
	//カメラ座標
	float3 eye;							
};

//座標変換用スロット
cbuffer Transform : register(b1)
{
	//ワールド変換行列
	matrix world;						
	//ボーン毎の変換行列
	matrix bone[256];					
}

//パイプラインステートから流れてくるデータを受け止める構造体
struct Output
{
	//システム用座標
	float4 svpos	: SV_POSITION;		
	//座標
	float4 pos		: POSITION;			
	//法線
	float4 normal	: NORMAL;			
	//テクスチャのUV値
	float2 uv		: TEXCOORD;
	//正接	
	float3 tangent	: TANGENT;
	//カラー		
	float4 color	: COLOR;
	//頂点に影響を与えるボーンのID（頂点1個当たり4本）
	uint4  ids		: IDs;			
	//頂点に影響を与えるボーンの重み（頂点1個当たり4本）
	float4 weight	: WEIGHT;			
	//カメラ座標からの距離（ぼやける処理に使用）
	float3 dis		: DISFROMEYE;
};