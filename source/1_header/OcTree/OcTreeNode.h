#pragma once
#include "Application.h"

/// <summary>
/// 八分木を構成するノード
/// </summary>
class BoxCollider;
class IFBX;
class Vector3;
class OcTreeNode
{
public:
	//コンストラクタ
	OcTreeNode(const shared_ptr<BoxCollider> col, int capacity);
	//デストラクタ
	~OcTreeNode();

	//オブジェクトを追加する関数
	bool AddObject(const shared_ptr<IFBX> obj);

	//空間を分割する関数
	void SubDivide();

	//子ノードに分割する関数
	void AddToChild(const vector<shared_ptr<IFBX>> objs);

	//クエリ範囲にあるオブジェクトを取得する関数
	vector<shared_ptr<IFBX>> Get(const shared_ptr<BoxCollider> bounds)noexcept;
private:
	//管理する空間
	shared_ptr<BoxCollider> _col;

	//子ノード
	vector<shared_ptr<OcTreeNode>> _children;

	//格納するオブジェクト
	vector<shared_ptr<IFBX>> _objs;

	//容量
	int _capacity;

	//1ならそのまま、0なら-1を返す関数
	int Convert(int x);
};