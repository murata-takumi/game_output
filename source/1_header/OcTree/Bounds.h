#pragma once
#include "Application.h"
#include "Vector3.h"

/// <summary>
/// 3D空間における矩形を表すクラス
/// </summary>
class Bounds
{
private:
	//中心座標
	XMFLOAT3 _pos;

	//幅、高さ、奥行
	XMFLOAT3 _halfLength;

public:

	//コンストラクタ
	Bounds(XMFLOAT3 pos, XMFLOAT3 halfLength);

	//座標が矩形の中にあるか判定する関数
	bool CheckPointInBounds(XMFLOAT3 point);

	//座標が矩形の中にあるか判定する関数（XMVector版）
	bool CheckPointInBounds(Vector3& vec);

	//中心座標を返す
	XMFLOAT3 Pos()const;

	//半分長さを返す
	XMFLOAT3 HalfLength()const;
};
