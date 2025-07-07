#pragma once
#include "Application.h"

/// <summary>
/// 当たり判定の派生元となるインターフェース
/// </summary>
class IFbx;
class Vector3;
class ICollider
{
private:

public:
	//コンストラクタ、デストラクタ
	ICollider() = default;
	~ICollider() = default;

	//更新処理
	virtual void Update(const XMMATRIX& mat) = 0;

	//中心座標を返す
	virtual shared_ptr<Vector3> Center()const = 0;

	//オブジェクトを返す
	virtual IFbx& Object()const = 0;
};