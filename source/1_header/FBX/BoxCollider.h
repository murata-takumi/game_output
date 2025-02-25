#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class FBXBase;
class BoxCollider
{
private:
	//この当たり判定を持つオブジェクト
	FBXBase& _object;

	//頂点の初期値
	vector<XMVECTOR> _initVerts;
	//画面に表示する頂点
	vector<XMVECTOR> _verts;
	//中心ベクトルの初期値
	XMVECTOR _initCenter;
	//中心ベクトル
	XMVECTOR _center;

	//それぞれ幅、高さ、奥行の長さの半分
	float _halfWidth, _halfHeight, _halfDepth;

	//当たり判定が持つ三方向のベクトルを設定する関数
	void SetVec(const XMVECTOR& vec);

public:
	//それぞれ中心、正面、右、上ベクトル
	XMVECTOR _frontDir, _rightDir, _upDir;

	//コンストラクタ
	BoxCollider(FBXBase& object, const XMFLOAT3& size);	
	//デストラクタ
	~BoxCollider();

	//座標を更新する関数
	void Update(const XMMATRIX& mat);

	//頂点を返す関数
	vector<XMVECTOR> Vertices()const;

	//幅、高さ、奥行を返す
	XMFLOAT3 HalfLength()const;

	//中心座標を返す
	XMVECTOR Center()const;

	//オブジェクトを返す
	FBXBase& Object()const;
};
