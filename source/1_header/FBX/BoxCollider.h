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
	//境界値
	map<string,float> _boundValues;
	//中心ベクトルの初期値
	XMVECTOR _initCenter;

	//正面ベクトル
	XMVECTOR _frontDir;
	//横ベクトル
	XMVECTOR _rightDir;
	//上ベクトル
	XMVECTOR _upDir;
	//オブジェクトの角度
	float _angle;

	//境界値を設定する関数
	void SetBoundValues();								

public:

	//更新される中心ベクトル
	XMVECTOR _center;

	//コンストラクタ
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//デストラクタ
	~BoxCollider();

	//座標を更新する関数
	void Update(const XMMATRIX& mat,float angle = 0.0f);
	
	//向きを設定する関数
	void SetDir(const XMVECTOR& dir);

	//頂点を返す関数
	vector<XMVECTOR> Vertices()const;	
	//境界値を返す関数
	map<string, float> BoundValues()const;
};
