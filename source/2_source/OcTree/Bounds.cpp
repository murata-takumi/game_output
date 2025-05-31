#include "OcTree/Bounds.h"

/// <summary>
/// コンストラクタ
/// 各種変数を初期化
/// </summary>
/// <param name="pos">座標</param>
/// <param name="halfLength">各座標軸の半分の長さ</param>
Bounds::Bounds(const Vector3& pos, const Vector3& halfLength) 
	:_pos(pos), _halfLength(halfLength)
{
	ImGuiManager::Instance().AddLabelAndFloat("HalfLenX", _halfLength.X());
}

/// <summary>
/// 座標が矩形の中にあるか判定する関数
/// </summary>
/// <param name="point">座標</param>
/// <returns>中にあるか</returns>
bool 
Bounds::CheckPointInBounds(Vector3& point)
{
	ImGuiManager::Instance().AddLabelAndFloat("len", _halfLength.X());
	//まずは座標を結ぶベクトルを取得
	auto a = Vector3(
		point.X() - _pos.X(),
		point.Y() - _pos.Y(),
		point.Z() - _pos.Z()
	);
	Vector3 vecBetPos = a;

	//半分の長さをXMVECTORに変換
	Vector3 halfLengthVec = _halfLength;
	float len = 0.0f;

	//ベクトルの各要素に対し処理を行う
	for (int i = 0; i < 3; i++)
	{
		//各座標軸に対する比率を求める（半分の長さピッタリだったら絶対値が1になる）
		auto s = XMVector3Dot(vecBetPos, XMMatrixIdentity().r[i]).m128_f32[0] / halfLengthVec[i];
		//比率の絶対値が1よりおおきければはみ出ているとみなし、中に含んでいない
		if (fabs(s) > 1)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// 中心座標を返す
/// </summary>
/// <returns></returns>
Vector3&
Bounds::Pos()
{
	return _pos;
}

/// <summary>
/// 半分長さを返す
/// </summary>
/// <returns></returns>
Vector3 
Bounds::HalfLength()const
{
	return _halfLength;
}