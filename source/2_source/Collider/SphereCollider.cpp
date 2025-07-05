#pragma once
#include "Includes.h"
#include "Vector3.h"

#include "Collider/SphereCollider.h"
#include "Collider/ColliderComposition.h"
#include "FBX/IFbx.h"

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="radius">半径</param>
/// <param name="pos">初期座標</param>
/// <param name="object">紐づけるオブジェクト</param>
void 
SphereCollider::Init(float radius, const Vector3& pos, IFbx* object)
{
	//半径を設定
	_radius = radius;

	//コンポジションも初期化
	_colliderComp = make_shared<ColliderComposition>(pos, object);
}

/// <summary>
/// 毎フレームの更新を行う関数
/// 座標変換を行う
/// </summary>
/// <param name="mat">座標変換に用いる行列</param>
void
SphereCollider::Update(const XMMATRIX& mat)
{
	//中心座標を更新
	_colliderComp->Update(mat);
}

/// <summary>
/// 中心座標を返す関数
/// </summary>
/// <returns>ポインタ</returns>
shared_ptr<Vector3>
SphereCollider::Center()const
{
	return _colliderComp->Center();
}

/// <summary>
/// 当たり判定を持つオブジェクトを返す
/// </summary>
/// <returns>オブジェクト</returns>
IFbx&
SphereCollider::Object()const
{
	return _colliderComp->Object();
}

float 
SphereCollider::Radius()const
{
	return _radius;
}