#include "Vector3.h"

#include "Collider/ColliderComposition.h"
#include "Fbx/IFbx.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="pos">座標</param>
/// <param name="object">紐付けられるオブジェクト</param>
ColliderComposition::ColliderComposition(const Vector3& pos, IFbx* object)
{
	//渡されたアドレスを基にスマートポインタを生成
	//管理権をスマートポインタに渡すためdeleter?を渡す
	_object = shared_ptr<IFbx>(object, [](IFbx*)
	{

	});

	//中心を初期化
	_initCenter = make_shared<Vector3>(pos);
	_center = make_shared<Vector3>(*_initCenter);
}

void 
ColliderComposition::Update(const XMMATRIX& mat)
{
	//中心座標を更新
	*_center = XMVector3Transform(*_initCenter, mat);
}

/// <summary>
/// 中心座標を返す関数
/// </summary>
/// <returns></returns>
shared_ptr <Vector3>
ColliderComposition::Center()const
{
	return _center;
}

/// <summary>
/// 当たり判定を持つオブジェクトを返す
/// </summary>
/// <returns>オブジェクト</returns>
IFbx& 
ColliderComposition::Object()const
{
	return *_object;
}