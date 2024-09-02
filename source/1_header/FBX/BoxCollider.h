#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class BoxCollider
{
private:
	//頂点の初期値
	vector<XMVECTOR> _initVerts;
	//画面に表示する頂点
	vector<XMVECTOR> _verts;
	//中心ベクトルの初期値
	XMVECTOR _initCenter;

	/// <summary>
	/// ベクトルを設定する関数
	/// </summary>
	/// <param name="vec"></param>
	void SetVec(const XMVECTOR& vec)
	{
		//まずは正面ベクトル
		_frontDir = XMVector3Normalize(vec);

		//XZ平面に対し鉛直なベクトルをとり、それと正面ベクトルの外積を取り右ベクトルとする
		auto vertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		_rightDir = XMVector3Normalize(XMVector3Cross(_frontDir, vertical));

		//正面ベクトル、右ベクトルの外積を上ベクトルとする
		_upDir = XMVector3Normalize(XMVector3Cross(_frontDir, _rightDir));
		//反転も忘れずに
		_upDir *= -1.0f;
	}

public:

	//それぞれ中心、正面、右、上ベクトル
	XMVECTOR _center, _frontDir, _rightDir, _upDir;

	//それぞれ幅、高さ、奥行の長さの半分
	float _halfWidth, _halfHeight, _halfDepth;

	//コンストラクタ
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//デストラクタ
	~BoxCollider();

	//座標を更新する関数
	void Update(const XMMATRIX& mat);

	//頂点を返す関数
	vector<XMVECTOR> Vertices()const;
};
