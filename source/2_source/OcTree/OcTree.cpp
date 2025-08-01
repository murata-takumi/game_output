#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/ICollider.h"
#include "FBX/IFbx.h"
#include "OcTree/OcTree.h"
#include "OcTree/OcTreeNode.h"

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
OcTree&
OcTree::Instance()
{
	static OcTree instance;
	return instance;
}

/// <summary>
/// コンストラクタとデストラクタ
/// どっちも使わない
/// </summary>
OcTree::OcTree()
{

}
OcTree::~OcTree()
{

}

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="col">管理する空間</param>
/// <param name="capacity">ノードが持てる最大容量</param>
void
OcTree::Init(const shared_ptr<ICollider> col, int capacity)
{
	//ルートノードを初期化
	_rootNode = make_shared<OcTreeNode>(col, capacity);
}

/// <summary>
/// オブジェクトを追加する関数
/// </summary>
/// <param name="obj">追加するオブジェクト</param>
/// <returns>追加で来たかどうか</returns>
bool 
OcTree::AddObject(const shared_ptr<IFbx> obj)
{
	//ルートノードに追加処理
	return _rootNode->AddObject(obj);
}

/// <summary>
/// クエリ範囲にあるオブジェクトを取得する関数
/// </summary>
/// <param name="bounds">クエリ範囲</param>
/// <returns>取得したオブジェクト</returns>
vector<shared_ptr<IFbx>>
OcTree::Get(const shared_ptr<ICollider> col)noexcept
{
	return _rootNode->Get(col);
}