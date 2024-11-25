#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class BoxCollider2D
{
private:
	//中心ベクトルの初期値
	XMVECTOR _initCenter;

public:
	RECT _rect;

	//それぞれ中心、右、上ベクトル
	XMVECTOR _center, _rightDir, _upDir;

	//それぞれ幅、高さ、奥行の長さの半分
	float _halfWidth, _halfHeight;

	//コンストラクタ
	BoxCollider2D(const XMVECTOR& size, const XMVECTOR& center);
	//デストラクタ
	~BoxCollider2D();

	//座標を更新する関数
	void Update();
};