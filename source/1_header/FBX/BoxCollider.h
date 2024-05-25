#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class BoxCollider
{
private:
	//画面に表示する頂点
	vector<XMVECTOR> _verts;					
	//頂点の初期値
	vector<XMVECTOR> _initVerts;					
	//境界値
	map<string,float> _boundValues;

	//境界値を設定する関数
	void SetBoundValues();								

public:
	//コンストラクタ
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//デストラクタ
	~BoxCollider();				

	//座標を更新する関数
	void Update(const XMMATRIX& mat);	

	//頂点を返す関数
	vector<XMVECTOR> Vertices()const;	
	//境界値を返す関数
	map<string, float> BoundValues()const;
};
