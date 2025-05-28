#include "OcTree/OcTreeNode.h"

#include <bitset>

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="bounds">扱う空間</param>
/// <param name="capacity">容量</param>
OcTreeNode::OcTreeNode(Bounds bounds, int capacity):
	_bounds(bounds),_capacity(capacity)
{

}

/// <summary>
/// デストラクタ
/// </summary>
OcTreeNode::~OcTreeNode()
{

}

/// <summary>
/// オブジェクトを追加する関数
/// </summary>
/// <param name="obj">追加するオブジェクト</param>
/// <returns></returns>
bool
OcTreeNode::AddObject(const shared_ptr<FBXObject> obj)
{
	//空間内に入ってなかったら処理中断
	if (!_bounds.CheckPointInBounds(obj.get()->Pos()))
	{
		return false;
	}

	//ベクトルにオブジェクト追加
	_objs.push_back(obj);

	//オブジェクト数が容量を上回ったら子ノード作成、分配
	if (_objs.size() > _capacity)
	{
		SubDivide();

		AddToChild(_objs);

		//分配したらベクトルは初期化
		_objs.clear();
	}

	return true;
}

/// <summary>
/// 管理する空間を8つに分割し、生成した子ノードに管理させる関数
/// </summary>
void
OcTreeNode::SubDivide()
{
	// 空間の半分長さを取得し、更に半分にする
	auto bh = _bounds.HalfLength();
	Vector3 halfVec = XMVectorScale(bh, 1.0f / 2.0f);

	//新しい矩形の座標に使用
	float quaterLength = halfVec.X() / 2.0f;
	for (int i = 0; i < 8; i++)
	{
		//iをビットに変換し、各桁をx,y,z座標に使用
		std::bitset<3> decimal(i);
		string decStr = decimal.to_string();

		int x = Convert(decStr[0]) * quaterLength;
		int y = Convert(decStr[1]) * quaterLength;
		int z = Convert(decStr[2]) * quaterLength;

		//新しい矩形を作成
		Bounds newBounds = Bounds(halfVec, Vector3(x, y, z));

		//子ノードを作成し、ベクトルに格納
		_children.push_back(OcTreeNode(newBounds,_capacity));
	}
}

/// <summary>
/// 子ノードにオブジェクトを追加する関数
/// </summary>
/// <param name="objs">オブジェクト</param>
void
OcTreeNode::AddToChild(vector<shared_ptr<FBXObject>> objs)
{
	for(auto& obj : objs)
	{
		for (int i = 0; i < 8; i++)
		{
			if (_children[i].AddObject(obj))
			{
				break;
			}
		}
	}
}

/// <summary>
/// クエリ範囲内に含まれているオブジェクトを全て取得する関数
/// </summary>
/// <param name="bounds">クエリ範囲</param>
/// <returns>オブジェクト</returns>
vector<shared_ptr<FBXObject>>
OcTreeNode::Get(Bounds bounds)
{
	vector<shared_ptr<FBXObject>> ret;

	//全オブジェクトに対しクエリ範囲内に含まれているかチェックを行う
	for (auto& obj : _objs)
	{
		if (bounds.CheckPointInBounds(obj.get()->Pos()))
		{
			ret.push_back(obj);
		}
	}

	//子ノードに対し再帰的に取得処理を行う
	for (OcTreeNode node : _children)
	{
		ret.insert(ret.end(), node.Get(bounds).begin(), node.Get(bounds).end());
	}

	return ret;
}

/// <summary>
/// 1ならそのまま、0なら-1に変換する関数
/// </summary>
/// <param name="x">1or0</param>
/// <returns>1or-1</returns>
int
OcTreeNode::Convert(int x)
{
	return x == 1 ? x : -1;
}