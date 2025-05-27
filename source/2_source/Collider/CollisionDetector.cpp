#include "Collider/CollisionDetector.h"

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
CollisionDetector&
CollisionDetector::Instance()
{
	static CollisionDetector instance;
	return instance;
}

/// <summary>
/// 二つのOBBが衝突しているか確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているかどうか</returns>
bool
CollisionDetector::CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2)
{
	//各OBBの右、上、正面ベクトル
	Vector3 center1 = col1.Center();
	Vector3 center2 = col2.Center();
	Vector3 rightLen1 = col1._rightDir * col1.HalfLength().x;
	Vector3 upLen1 = col1._upDir * col1.HalfLength().y;
	Vector3 frontLen1 = col1._frontDir * col1.HalfLength().z;
	Vector3 rightLen2 = col2._rightDir * col2.HalfLength().x;
	Vector3 upLen2 = col2._upDir * col2.HalfLength().y;
	Vector3 frontLen2 = col2._frontDir * col2.HalfLength().z;

	//双方のOBBの中心を結ぶベクトル
	Vector3 vecBetCenter = center2 - center1;

	//obb1_front
	float r = LenOnSeparateAxis(col1._frontDir, rightLen2, upLen2, frontLen2);
	float s = XMVector3Dot(vecBetCenter, col1._frontDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().z)
	{
		return false;
	}

	auto diffZ = r + col1.HalfLength().z - fabs(s);
	if (s <= 0)
	{
		diffZ *= -1.0f;
	}

	//obb1_right
	r = LenOnSeparateAxis(col1._rightDir, rightLen2, upLen2, frontLen2);
	s = XMVector3Dot(vecBetCenter, col1._rightDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().x)
	{
		return false;
	}

	auto diffX = r + col1.HalfLength().x - fabs(s);
	if (s <= 0)
	{
		diffX *= -1.0f;
	}

	//obb1_up
	r = LenOnSeparateAxis(col1._upDir, rightLen2, upLen2, frontLen2);
	s = XMVector3Dot(vecBetCenter, col1._upDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().y)
	{
		return false;
	}

	auto diffY = r + col1.HalfLength().y - fabs(s);

	//obb2_front
	r = LenOnSeparateAxis(col2._frontDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._frontDir).m128_f32[0]);
	if (s > r + col2.HalfLength().z)
	{
		return false;
	}

	//obb2_right
	r = LenOnSeparateAxis(col2._rightDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._rightDir).m128_f32[0]);
	if (s > r + col2.HalfLength().x)
	{
		return false;
	}

	//obb2_up
	r = LenOnSeparateAxis(col2._upDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._upDir).m128_f32[0]);
	if (s > r + col2.HalfLength().y)
	{
		return false;
	}

	//最小値を更新
	auto min = fabs(diffX);
	if (min > fabs(diffY))
	{
		min = fabs(diffY);
	}
	if (min > fabs(diffZ))
	{
		min = fabs(diffZ);
	}

	//最小値の座標軸に対しめり込みを戻す処理を行う
	if (FLT_EPSILON < fabs(diffX) && fabs(min - fabs(diffX)) < FLT_EPSILON)
	{
		col2.Object().Pos().X() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().x;
	}
	else if (FLT_EPSILON < fabs(diffY) && fabs(min - fabs(diffY)) < FLT_EPSILON)
	{
		col2.Object().Pos().Y() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().y;
	}
	else if (FLT_EPSILON < fabs(diffZ) && fabs(min - fabs(diffZ)) < FLT_EPSILON)
	{
		col2.Object().Pos().Z() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().z;
	}

	return true;
}

/// <summary>
/// OBBに座標が入り込んでいるか確認する関数
/// </summary>
/// <param name="col">対象のOBB</param>
/// <param name="point">座標</param>
/// <returns>入り込んでいるかどうか</returns>
bool
CollisionDetector::CheckColAndPoint(const BoxCollider& col, const Vector3& point)
{
	//座標とOBBの中心を結ぶベクトルを取得
	auto vecBetcolAndPoint = col.Center() - point;

	//方向ベクトルからはみ出ているベクトル
	Vector3 vec = XMVectorSet(0, 0, 0, 0);

	//方向ベクトルに対する比率を取得し、絶対値が1より大きければ（=はみ出ていたら）距離に加算
	float s = XMVector3Dot(vecBetcolAndPoint, col._rightDir).m128_f32[0] / col.HalfLength().x;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().x * col._rightDir;
	}

	s = XMVector3Dot(vecBetcolAndPoint, col._upDir).m128_f32[0] / col.HalfLength().y;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().y * col._upDir;
	}

	s = XMVector3Dot(vecBetcolAndPoint, col._frontDir).m128_f32[0] / col.HalfLength().z;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().z * col._frontDir;
	}

	//距離が0以下だったらOBBの中に座標が入り込んでいる
	if (XMVector3Length(vec).m128_f32[0] <= 0)
	{
		return true;
	}

	return false;
}

/// <summary>
/// OBBに直線が入っているか確認する関数
/// </summary>
/// <param name="col">OBB</param>
/// <param name="startPos">直線の始点</param>
/// <param name="direction">直線の向き</param>
/// <returns></returns>
bool
CollisionDetector::CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& direction)
{
	return true;
}

/// <summary>
/// 分離軸に対し投影された各ベクトルの大きさの和を求める関数
/// </summary>
/// <param name="sep">分離軸</param>
/// <param name="right">右ベクトル</param>
/// <param name="up">上ベクトル</param>
/// <param name="front">正面ベクトル</param>
/// <returns>ベクトルの大きさの和</returns>
float
CollisionDetector::LenOnSeparateAxis(const Vector3& sep, const Vector3& right, const Vector3& up, const Vector3& front)
{
	//分離軸と各ベクトルの内積を取り、その絶対値が投影された大きさとなる
	float r1 = fabs(XMVector3Dot(sep, right).m128_f32[0]);
	float r2 = fabs(XMVector3Dot(sep, up).m128_f32[0]);
	float r3 = fabs(XMVector3Dot(sep, front).m128_f32[0]);

	return r1 + r2 + r3;
}