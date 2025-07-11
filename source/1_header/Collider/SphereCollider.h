#pragma once
#include "Application.h"

#include "Collider/ICollider.h"

/// <summary>
/// 球状の当たり判定を実装するクラス
/// </summary>
class ColliderComposition;
class IFbx;
class Vector3;
class SphereCollider : public ICollider
{
private:
	//コンポジションクラス
	shared_ptr<ColliderComposition> _colliderComp;

	//半径
	float _radius;

public:
	SphereCollider() = default;
	~SphereCollider() = default;

	//初期化関数
	void Init(float radius, const Vector3& pos = Vector3(0, 0, 0),
		IFbx* object = nullptr);

	//座標を更新する関数
	void Update(const XMMATRIX & mat);

	//中心座標を返す
	shared_ptr<Vector3> Center()const;

	//オブジェクトを返す
	IFbx& Object()const;

	//半径
	float Radius()const;
};