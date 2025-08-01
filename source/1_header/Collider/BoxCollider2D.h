#pragma once
#include "Application.h"

/// <summary>
/// 矩形の当たり判定を実装するクラス
/// </summary>
class Vector3;
class BoxCollider2D
{
private:
	//中心ベクトルの初期値
	Vector3 _initCenter;

	RECT _rect;

	//それぞれ中心、右、上ベクトル
	Vector3 _center, _rightDir, _upDir;

public:
	//それぞれ幅、高さ、奥行の長さの半分
	float _halfWidth, _halfHeight;

	//コンストラクタ
	BoxCollider2D(const Vector3& size, const Vector3& center);
	//デストラクタ
	~BoxCollider2D();

	//座標を更新する関数
	void Update();

	//矩形を返す
	RECT Rect();

	//文字列の適した座標を返す
	XMFLOAT2 StrCenterPos(string& str);
};