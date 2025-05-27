#pragma once
#include "Application.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// 当たり判定の計算を管理するクラス
/// </summary>
class CollisionDetector
{
public:
	static CollisionDetector& Instance();

	//OBB同士の当たり判定を確認する関数
	bool CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2);

	//OBBにベクトルが入っているか確認する関数
	bool CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& direction);

	//OBBに座標が入っているか確認する関数
	bool CheckColAndPoint(const BoxCollider& col, const Vector3& point);

	//分離軸に投影された線分の長さを取得する
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& right, const Vector3& up, const Vector3& front = XMVectorZero());
};