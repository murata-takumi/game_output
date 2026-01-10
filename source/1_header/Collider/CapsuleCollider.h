#pragma once
#include "Application.h"

#include "Collider/ICollider.h"

/// <summary>
/// カプセル型の当たり判定を実装するクラス
/// </summary>
class ColliderComposition;
class IFbx;
class ImGuiManager;
class Vector3;
class CapsuleCollider : public ICollider
{
private:
	//コンポジションクラス
	shared_ptr<ColliderComposition> _colliderComp;

	//それぞれ中心線の両端
	shared_ptr <Vector3> _upEdge;
	shared_ptr <Vector3> _downEdge;

	//高さと半径
	float _height, _radius;

public:

	//コンストラクタ
	CapsuleCollider() = default;
	//デストラクタ
	~CapsuleCollider() = default;

	void Init(float height, float radius, const Vector3& pos = Vector3(0, 0, 0),
		IFbx* object = nullptr);

	//座標を更新する関数
	void Update(const XMMATRIX& mat);

	//高さ
	float Height()const;

	//半径
	float Radius()const;

	//中心座標を返す
	shared_ptr<Vector3> Center()const;

	//中心線の両端
	shared_ptr<Vector3> UpEdge()const;
	shared_ptr<Vector3> DownEdge()const;

	//オブジェクトを返す
	IFbx& Object()const;
};