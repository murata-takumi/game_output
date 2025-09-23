#pragma once
#include "Application.h"

/// <summary>
/// 当たり判定の計算を管理するクラス
/// </summary>
class BoxCollider;
class Dx12Wrapper;
class FbxActor;
class ICollider;
class IFbx;
class FbxObject;
class Vector3;
class CollisionDetector
{
public:
	//インスタンスを返す
	static CollisionDetector& Instance();

	//OBBと座標の間の距離を取得する関数
	float GetLengthBetweenColAndPos(
		shared_ptr<ICollider> col, 
		const Vector3& dir, 
		const Vector3& pos
	);

	//連続的な衝突判定処理
	bool CheckContinuousCollisionDetection(
		IFbx* actor,
		shared_ptr<ICollider> col, 
		const Vector3& dir, 
		const Vector3& currentPos,
		const float speed
	);

	//Collider同士の当たり判定を確認する関数
	bool CheckColAndCol(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2);

	//OBBにベクトルが入っているか確認する関数
	Vector3 CheckColAndVec(
		shared_ptr<ICollider> col, 
		const Vector3& startPos, 
		const Vector3& endPos
	);

	//OBBに座標が入っているか確認する関数
	bool CheckColAndPoint(shared_ptr<ICollider> col, const Vector3& point);

private:
	//矩形同士の当たり判定を確認する関数
	bool CheckBoxAndBox(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter);

	//球形同士の当たり判定を確認する関数
	bool CheckSphereAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter);

	//矩形と球形の当たり判定を確認する関数
	bool CheckBoxAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter);


	//分離軸に投影された線分の長さを取得する
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec);
};