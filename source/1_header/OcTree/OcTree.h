#pragma once
#include "Application.h"

/// <summary>
/// 八分木を管理するクラス
/// </summary>
class BoxCollider;
class ICollider;
class IFbx;
class OcTreeNode;
class OcTree
{
public:
	static OcTree& Instance();

	//初期化関数
	void Init(const shared_ptr<ICollider> col, int capacity);

	//オブジェクトを追加する関数
	bool AddObject(const shared_ptr<IFbx> obj);

	//クエリ範囲にあるオブジェクトを取得する関数
	vector<shared_ptr<IFbx>> GetByCollider(const shared_ptr<ICollider> col)noexcept;

	//ベクトルに接しているオブジェクトを取得する関数
	vector<shared_ptr<IFbx>> GetByVector(const Vector3 startPos, const Vector3 direction, float length)noexcept;
private:
	//ノード
	shared_ptr<OcTreeNode> _rootNode;

	//コンストラクタ、デストラクタ
	OcTree();
	OcTree(const OcTree&) = delete;
	~OcTree();
};