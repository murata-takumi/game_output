#include "OcTree/Bounds.h"

/// <summary>
/// コンストラクタ
/// 各種変数を初期化
/// </summary>
/// <param name="pos">座標</param>
/// <param name="halfLength">各座標軸の半分の長さ</param>
Bounds::Bounds(XMFLOAT3 pos, XMFLOAT3 halfLength) 
	:_pos(pos), _halfLength(halfLength)
{

}

/// <summary>
/// 座標が矩形の中にあるか判定する関数
/// </summary>
/// <param name="point">座標</param>
/// <returns>中にあるか</returns>
bool 
Bounds::CheckPointInBounds(XMFLOAT3 point)
{
	//まずは座標を結ぶベクトルを取得
	auto a = XMFLOAT3(
		point.x - _pos.x,
		point.y - _pos.y,
		point.z - _pos.z
	);
	Vector3 vecBetPos = XMLoadFloat3(&a);

	//半分の長さをXMVECTORに変換
	Vector3 halfLengthVec = XMLoadFloat3(&_halfLength);

	//ベクトルの各要素に対し処理を行う
	for (int i = 0; i < 3; i++)
	{
		//各座標軸に対する比率を求める（半分の長さピッタリだったら絶対値が1になる）
		auto s = XMVector3Dot(vecBetPos, XMMatrixIdentity().r[i]) / halfLengthVec[i];
		//比率の絶対値が1よりおおきければはみ出ているとみなし、中に含んでいない
		if (fabs(s.m128_f32[0]) > 1)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// 座標が矩形の中にあるか判定する関数（XMVector版）
/// </summary>
/// <param name="vec">座標ベクトル</param>
/// <returns>中にあるか</returns>
bool 
Bounds::CheckPointInBounds(Vector3& vec)
{
	//XMVectorからXMFLOAT3に変換したうえで渡す
	XMFLOAT3 temp = XMFLOAT3(vec.X(), vec.Y(), vec.Z());

	return CheckPointInBounds(temp);
}

/// <summary>
/// 中心座標を返す
/// </summary>
/// <returns></returns>
XMFLOAT3 
Bounds::Pos()const
{
	return _pos;
}

/// <summary>
/// 半分長さを返す
/// </summary>
/// <returns></returns>
XMFLOAT3 
Bounds::HalfLength()const
{
	return _halfLength;
}