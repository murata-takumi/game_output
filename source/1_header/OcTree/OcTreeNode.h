#pragma once
#include "Application.h"

#include "Collider/CollisionDetector.h"
#include "Collider/BoxCollider.h"
#include "FBX/FBXObject.h"
#include "OcTree/Bounds.h"

/// <summary>
/// 八分木を構成するノード
/// </summary>
class OcTreeNode
{
public:
	//コンストラクタ
	OcTreeNode(const shared_ptr<Bounds> bounds, int capacity);
	//デストラクタ
	~OcTreeNode();

	//オブジェクトを追加する関数
	bool AddObject(const shared_ptr<FBXObject> obj);

	//空間を分割する関数
	void SubDivide();

	//子ノードに分割する関数
	void AddToChild(const vector<shared_ptr<FBXObject>> objs);

	//クエリ範囲にあるオブジェクトを取得する関数
	vector<shared_ptr<FBXObject>> Get(const shared_ptr<BoxCollider> bounds);
private:
	//管理する空間
	shared_ptr<Bounds> _bounds;

	//子ノード
	vector<shared_ptr<OcTreeNode>> _children;

	//格納するオブジェクト
	vector<shared_ptr<FBXObject>> _objs;

	//容量
	int _capacity;

	//1ならそのまま、0なら-1を返す関数
	int Convert(int x);
};