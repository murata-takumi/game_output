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
/// 分離軸定理二つのOBBが衝突しているか確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているかどうか</returns>
bool
CollisionDetector::CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2)
{
	return CheckOBBIntersection(col1, col2) && CheckOBBIntersection(col2, col1);
}

/// <summary>
/// OBBに線分が入っているか確認する関数
/// </summary>
/// <param name="col">OBB</param>
/// <param name="startPos">線分の始点</param>
/// <param name="endPos">線分の終点</param>
/// <returns></returns>
bool
CollisionDetector::CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& endPos)
{
	//線分の中心を取得
	Vector3 lineCenter = (startPos + endPos) / 2;
	//線分の向きも取得
	Vector3 lineDir = endPos - startPos;
	//線分の半分の長さ
	float lineExtent = XMVector3Length(lineDir).m128_f32[0] * 0.5f;

	//線分が短すぎた場合は点として扱う
	if (lineExtent < FLT_EPSILON)
	{
		//単位ベクトル
		lineDir = Vector3(1, 0, 0);
		lineExtent = 0.0f;
	}
	else
	{
		lineDir = XMVector3Normalize(endPos - startPos);
	}

	//OBBと線分の中心の差分
	Vector3 centerDiff = lineCenter - col.Center();

	float r, r0, r1;
	for (int i = 0; i < 3; i++)
	{
		//r = abs(XMVector3Dot(centerDiff,col.))
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

	for (int i = 0; i < 3; i++)
	{
		//方向ベクトルに対する比率を取得し、絶対値が1より大きければ（=はみ出ていたら）距離に加算
		float s = XMVector3Dot(vecBetcolAndPoint, col.DirectionVectors()[i]).m128_f32[0] / col.HalfLength()[i];
		if (fabs(s) > 1)
		{
			vec += (1 - fabs(s)) * col.HalfLength()[i] * col.DirectionVectors()[i];
		}
	}

	//距離が0以下だったらOBBの中に座標が入り込んでいる
	if (XMVector3Length(vec).m128_f32[0] <= 0)
	{
		return true;
	}

	return false;
}

bool 
CollisionDetector::CheckOBBIntersection(const BoxCollider& col1, const BoxCollider& col2)
{
	//各OBBの右、上、正面ベクトル
	Vector3 center1 = col1.Center();
	Vector3 center2 = col2.Center();

	//双方のOBBの中心を結ぶベクトル
	Vector3 vecBetCenter = center2 - center1;

	for (int i = 0; i < 3; i++)
	{
		//各方向ベクトルの分離軸への投影の和
		float r = 0;
		for (int j = 0; j < 3; j++)
		{
			r += LenOnSeparateAxis(col1.DirectionVectors()[i], col2.DirectionVectors()[j] * col2.HalfLength()[j]);
		}

		//中心間ベクトルの分離軸への投影
		float s = XMVector3Dot(vecBetCenter, col1.DirectionVectors()[i]).m128_f32[0];

		//比較して、中心間ベクトルの投影の方が大きければ衝突していない
		if (fabs(s) > r + col1.HalfLength()[i])
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// 分離軸に対し投影されたベクトルの大きさを求める関数
/// </summary>
/// <param name="sep">分離軸</param>
/// <param name="dirVec">方向ベクトル</param>
/// <returns>分離軸上の方向ベクトルの大きさ</returns>
float
CollisionDetector::LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec)
{
	//分離軸と各ベクトルの内積を取り、その絶対値が投影された大きさとなる
	float ret = fabs(XMVector3Dot(sep, dirVec).m128_f32[0]);

	return ret;
}