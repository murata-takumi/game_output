#pragma once
#include "Application.h"

#include "Collider/BoxCollider.h"
#include "OcTree/OcTreeNode.h"

/// <summary>
/// 八分木を管理するクラス
/// </summary>
class OcTree
{
public:
	static OcTree& Instance();

	//初期化関数
	void Init(const shared_ptr<BoxCollider> col, int capacity);

	//オブジェクトを追加する関数
	bool AddObject(const shared_ptr<FBXObject> obj);

	//クエリ範囲にあるオブジェクトを取得する関数
	vector<shared_ptr<FBXObject>> Get(const shared_ptr<BoxCollider> col);

private:
	//ノード
	shared_ptr<OcTreeNode> _rootNode;

	//コンストラクタ、デストラクタ
	OcTree();
	OcTree(const OcTree&) = delete;
	~OcTree();
};