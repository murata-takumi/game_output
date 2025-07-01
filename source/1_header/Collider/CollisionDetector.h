#pragma once
#include "Application.h"

/// <summary>
/// 当たり判定の計算を管理するクラス
/// </summary>
class BoxCollider;
class Dx12Wrapper;
class FBXActor;
class IFBX;
class FBXObject;
class Vector3;
class CollisionDetector
{
public:
	//インスタンスを返す
	static CollisionDetector& Instance();

	//OBBと座標の間の距離を取得する関数
	float GetLengthBetweenColAndPos(
		const BoxCollider& col, 
		const Vector3& dir, 
		const Vector3& pos
	);

	//連続的な衝突判定処理
	bool CheckContinuousCollisionDetection(
		const BoxCollider& col, 
		const Vector3& dir, 
		const Vector3& currentPos,
		const float speed
	);

	//OBB同士の当たり判定を確認する関数
	bool CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2);

	//OBBにベクトルが入っているか確認する関数
	bool CheckColAndVec(
		const BoxCollider& col, 
		const Vector3& startPos, 
		const Vector3& endPos
	);

	//OBBに座標が入っているか確認する関数
	bool CheckColAndPoint(const BoxCollider& col, const Vector3& point);

private:
	//ループを回してOBB同士が接触しているか判断する
	bool CheckOBBIntersection(const BoxCollider& col1, const BoxCollider& col2);

	//分離軸に投影された線分の長さを取得する
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec);
};