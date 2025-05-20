#pragma once
#include "Application.h"

#include "FBX/BoxCollider.h"
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
	bool CheckColAndVec(const BoxCollider& col, const XMVECTOR& startPos, const XMVECTOR& direction);

	//OBBに座標が入っているか確認する関数
	bool CheckColAndPoint(const BoxCollider& col, const XMVECTOR& point);

	//分離軸に投影された線分の長さを取得する
	float LenOnSeparateAxis(const XMVECTOR& sep, const XMVECTOR& right, const XMVECTOR& up, const XMVECTOR& front = XMVectorZero());
};