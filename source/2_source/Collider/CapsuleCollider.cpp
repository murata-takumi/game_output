#pragma once
#include "Includes.h"
#include "Vector3.h"

#include "Collider/CapsuleCollider.h"
#include "Collider/ColliderComposition.h"
#include "FBX/IFbx.h"
#include "Manager/ImGuiManager.h"

/// <summary>
/// 当たり判定初期化処理
/// </summary>
/// <param name="height">高さ</param>
/// <param name="radius">半径</param>
/// <param name="center">当たり判定の中心ベクトル</param>
/// <param name="object">紐づけるオブジェクト</param>
void 
CapsuleCollider::Init(float height, float radius, const Vector3& pos,IFbx* object)
{
	_colliderComp = make_shared<ColliderComposition>(pos, object);

	_height = height;
	_radius = radius;

	_upEdge = make_shared<Vector3>(*_colliderComp->Center() + Vector3(0, _height - _radius * 20, 0));
	_downEdge = make_shared<Vector3>(*_colliderComp->Center() + Vector3(0, _height + _radius * 20, 0));
}

/// <summary>
/// 毎フレームの更新を行う関数
/// 座標変換を行う
/// </summary>
/// <param name="mat">座標変換に用いる行列</param>
void 
CapsuleCollider::Update(const XMMATRIX& mat)
{
	//中心座標、両端座標を更新
	_colliderComp->Update(mat);
	_upEdge = make_shared<Vector3>(*_colliderComp->Center() + Vector3(0, (_height - _radius * 2) / 2, 0));
	_downEdge = make_shared<Vector3>(*_colliderComp->Center() - Vector3(0, (_height - _radius * 2) / 2, 0));
}

/// <summary>
/// 高さを返す
/// </summary>
/// <returns>高さ</returns>
float 
CapsuleCollider::Height()const
{
	return _height;
}

/// <summary>
/// 半径を返す
/// </summary>
/// <returns>半径</returns>
float
CapsuleCollider::Radius()const
{
	return _radius;
}

/// <summary>
/// 中心座標を返す関数
/// </summary>
/// <returns>ポインタ</returns>
shared_ptr<Vector3>
CapsuleCollider::Center()const
{
	return _colliderComp->Center();
}

/// <summary>
/// 中心線の両端（下）
/// </summary>
/// <returns>座標</returns>
shared_ptr<Vector3> 
CapsuleCollider::UpEdge()const
{
	return _upEdge;
}

/// <summary>
/// 中心線の両端（上）
/// </summary>
/// <returns>座標</returns>
shared_ptr<Vector3>
CapsuleCollider::DownEdge()const
{
	return _downEdge;
}

/// <summary>
/// 当たり判定を持つオブジェクトを返す
/// </summary>
/// <returns>オブジェクト</returns>
IFbx&
CapsuleCollider::Object()const
{
	return _colliderComp->Object();
}