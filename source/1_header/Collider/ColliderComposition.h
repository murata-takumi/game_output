#pragma once
#include "Application.h"

class IFbx;
class Vector3;
class ColliderComposition
{
private:
	//この当たり判定を持つオブジェクト
	shared_ptr<IFbx> _object;

	//中心ベクトルの初期値
	shared_ptr <Vector3> _initCenter;
	//中心ベクトル
	shared_ptr <Vector3> _center;

public:
	//コンストラクタ
	ColliderComposition(const Vector3& pos = Vector3(0, 0, 0),IFbx* object = nullptr);

	//座標を更新する関数
	void Update(const XMMATRIX& mat);

	//中心座標を返す
	shared_ptr <Vector3> Center()const;

	//オブジェクトを返す
	IFbx& Object()const;
};