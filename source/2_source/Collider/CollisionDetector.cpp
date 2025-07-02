#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "FBX/FbxActor.h"
#include "FBX/IFbx.h"
#include "FBX/FbxObject.h"
#include "Wrapper/Dx12Wrapper.h"

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
/// OBBと座標の間の距離を返す関数
/// </summary>
/// <param name="col">OBB</param>
/// <param name="dir">投影するベクトル</param>
/// <param name="pos">座標</param>
/// <returns>距離</returns>
float 
CollisionDetector::GetLengthBetweenColAndPos(const BoxCollider& col, const Vector3& dir, const Vector3& pos)
{
	//座標とOBBの中心の差分
	Vector3 diff = pos - col.Center();
	//差分のベクトルへの投影
	float lenOnDir = abs(XMVector3Dot(diff, dir).m128_f32[0]);

	//OBBの方向ベクトルのベクトルへの投影の和を取得
	float sumOfVecOnDir = 0.0f;
	for (int i = 0; i < 3; i++)
	{
		sumOfVecOnDir += col.HalfLength()[i] * abs(XMVector3Dot(dir, col.DirectionVectors()[i]).m128_f32[0]);
	}

	float lenBetColAndPos = lenOnDir - sumOfVecOnDir;

	return lenBetColAndPos;
}

/// <summary>
/// 連続的な衝突判定処理
/// </summary>
/// <param name="col">衝突対象のOBB</param>
/// <param name="dir">移動方向</param>
/// <param name="pos">現在の座標</param>
/// <param name="speed">移動速度</param>
/// <returns>衝突するかどうか</returns>
bool 
CollisionDetector::CheckContinuousCollisionDetection(
	const BoxCollider& col, 
	const Vector3& dir, 
	const Vector3& currentPos, 
	const float speed)
{
	//1フレーム後の座標を取得
	Vector3 nextPos = currentPos + (dir * 3 * speed * Dx12Wrapper::Instance().GetDeltaTime());

	//現在と1フレーム後の座標のベクトルがOBBと交わる（=1フレーム後に衝突する）なら真
	if (CheckColAndVec(col, currentPos, nextPos))
	{


		return true;
	}

	return false;
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
/// <returns>入っているかどうか</returns>
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
	//OBBの方向ベクトルに対し中心差分を投影し、半分長と線分の投影の和を比較して衝突判定
	for (int i = 0; i < 3; i++)
	{
		//中心差分の投影
		r = abs(XMVector3Dot(centerDiff, col.DirectionVectors()[i]).m128_f32[0]);
		//方向ベクトルの半分長
		r0 = col.HalfLength()[i];
		//線分の方向ベクトルへの投影
		r1 = lineExtent * abs(XMVector3Dot(lineDir, col.DirectionVectors()[i]).m128_f32[0]);
		if (r > r0 + r1)
		{
			return false;
		}
	}

	//各方向ベクトルへの投影の和を求め、線分方向ベクトルへの投影と半分長の和と比較
	//差分の線分ベクトルへの投影
	//r = abs(XMVector3Dot(centerDiff, lineDir).m128_f32[0]);
	////方向ベクトルへの投影の和
	//r0 = 0.0f;
	//for (int i = 0; i < 3; i++)
	//{
	//	r0 += col.HalfLength()[i] * abs(XMVector3Dot(lineDir,col.DirectionVectors()[i]).m128_f32[0]);
	//}
	//if (r > r0 + lineExtent)
	//{
	//	return false;
	//}

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

/// <summary>
/// OBB同士が衝突しているか判別する関数
/// </summary>
/// <param name="col1">OBBその1</param>
/// <param name="col2">OBBその2</param>
/// <returns>衝突しているか</returns>
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