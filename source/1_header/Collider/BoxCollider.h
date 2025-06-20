#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class FBXBase;
class Vector3;
class BoxCollider
{
private:
	//この当たり判定を持つオブジェクト
	shared_ptr<FBXBase> _object;

	//頂点の初期値
	vector<Vector3> _initVerts;
	//画面に表示する頂点
	vector<Vector3> _verts;

	//方向ベクトル
	Vector3* _directionVecs = new Vector3[3];

	//中心ベクトルの初期値
	Vector3 _initCenter;
	//中心ベクトル
	Vector3 _center;

	//それぞれ幅、高さ、奥行の長さの半分
	float _halfWidth, _halfHeight, _halfDepth;

	//当たり判定が持つ三方向のベクトルを設定する関数
	void SetVec(const Vector3& vec);

public:
	//コンストラクタ
	BoxCollider(const Vector3& size, const Vector3& pos = Vector3(0,0,0), FBXBase* object = nullptr);
	//デストラクタ
	~BoxCollider();

	//座標を更新する関数
	void Update(const XMMATRIX& mat);

	//頂点を返す関数
	vector<Vector3> Vertices()const;

	Vector3* DirectionVectors()const;

	//幅、高さ、奥行を返す
	Vector3 HalfLength()const;

	//中心座標を返す
	Vector3 Center()const;

	//オブジェクトを返す
	FBXBase& Object()const;
};
