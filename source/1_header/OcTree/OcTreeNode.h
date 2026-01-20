#pragma once
#include "Application.h"

/// <summary>
/// 八分木を構成するノード
/// </summary>
class BoxCollider;
class ICollider;
class IFbx;
class Vector3;
class OcTreeNode
{
public:
	//コンストラクタ
	OcTreeNode(const shared_ptr<ICollider> col, int capacity);
	//デストラクタ
	~OcTreeNode();

	//オブジェクトを追加する関数
	bool AddObject(const shared_ptr<IFbx> obj);

	//空間を分割する関数
	void SubDivide();

	//子ノードに分割する関数
	void AddToChild(const vector<shared_ptr<IFbx>> objs);

	//クエリ範囲にあるオブジェクトを取得する関数
	vector<shared_ptr<IFbx>> GetByCollider(const shared_ptr<ICollider> bounds)noexcept;

	//ベクトルに接しているオブジェクトを取得する関数
	vector<shared_ptr<IFbx>> GetByVector(const Vector3 startPos, const Vector3 direction, float length)noexcept;
private:
	//管理する空間
	shared_ptr<ICollider> _col;

	//子ノード
	vector<shared_ptr<OcTreeNode>> _children;

	//格納するオブジェクト
	vector<shared_ptr<IFbx>> _objs;

	//容量
	int _capacity;

	//1ならそのまま、0なら-1を返す関数
	int Convert(int x);
};