#pragma once
#include "Application.h"
#include "Vector3.h"

#include "Manager/ImGuiManager.h"

/// <summary>
/// 3D空間における矩形を表すクラス
/// </summary>
class Bounds
{
private:
	//中心座標
	Vector3 _pos;

	//幅、高さ、奥行
	Vector3 _halfLength;

public:

	//コンストラクタ
	Bounds(const Vector3& pos, const Vector3& halfLength);

	//座標が矩形の中にあるか判定する関数
	bool CheckPointInBounds(const Vector3& point);

	//中心座標を返す
	Vector3& Pos();

	//半分長さを返す
	Vector3 HalfLength()const;
};
