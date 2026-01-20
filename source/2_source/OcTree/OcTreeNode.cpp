#include <bitset>

#include "Vector3.h"

#include "Collider/CollisionDetector.h"
#include "Collider/BoxCollider.h"
#include "Collider/ICollider.h"
#include "FBX/IFbx.h"
#include "OcTree/OcTreeNode.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="bounds">扱う空間</param>
/// <param name="capacity">容量</param>
OcTreeNode::OcTreeNode(const shared_ptr<ICollider> col, int capacity):
	_col(col),_capacity(capacity)
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
OcTreeNode::AddObject(const shared_ptr<IFbx> obj)
{
	//空間内に入ってなかったら処理中断
	if (!CollisionDetector::Instance().GetLengthBetweenColAndPos(_col, obj.get()->CurrentPosition()) <= 0.0f)
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
	auto bh = dynamic_pointer_cast<BoxCollider>(_col)->HalfLength();

	//新しい矩形の座標に使用
	float halfLen = bh.X() / 2.0f;
	for (int i = 0; i < 8; i++)
	{
		//iをビットに変換し、各桁をx,y,z座標に使用
		std::bitset<3> decimal(i);
		string decStr = decimal.to_string();

		int x = Convert(decStr[0]) * halfLen;
		int y = Convert(decStr[1]) * halfLen;
		int z = Convert(decStr[2]) * halfLen;

		//新しい矩形を作成
		shared_ptr<BoxCollider> newCol = make_shared<BoxCollider>();
		newCol->Init(bh, Vector3(x, y, z));

		//子ノードを作成し、ベクトルに格納
		_children.push_back(make_shared<OcTreeNode>(newCol,_capacity));
	}
}

/// <summary>
/// 子ノードにオブジェクトを追加する関数
/// </summary>
/// <param name="objs">オブジェクト</param>
void
OcTreeNode::AddToChild(const vector<shared_ptr<IFbx>> objs)
{
	for(auto& obj : objs)
	{
		for (int i = 0; i < 8; i++)
		{
			if (_children[i]->AddObject(obj))
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
vector<shared_ptr<IFbx>>
OcTreeNode::GetByCollider(const shared_ptr<ICollider> col)noexcept
{
	vector<shared_ptr<IFbx>> ret;

	//全オブジェクトに対しクエリ範囲内に含まれているかチェックを行う
	for (auto& obj : _objs)
	{
		if (CollisionDetector::Instance().CheckColAndCol(col,obj->Collider()))
		{
			ret.push_back(obj);
		}
	}

	//子ノードに対し再帰的に取得処理を行う
	for (shared_ptr<OcTreeNode> node : _children)
	{
		const auto& child = node->GetByCollider(col);
		ret.insert(ret.end(), child.begin(), child.end());
	}

	return ret;
}

/// <summary>
/// ベクトルに接しているオブジェクトを取得する関数
/// </summary>
/// <param name="startPos">ベクトルの始点</param>
/// <param name="direction">ベクトルの向き</param>
/// <param name="length">ベクトルの長さ</param>
/// <returns>衝突しているオブジェクト</returns>
vector<shared_ptr<IFbx>> 
OcTreeNode::GetByVector(const Vector3 startPos, const Vector3 direction, float length)noexcept
{
	vector<shared_ptr<IFbx>> ret;

	//全オブジェクトに対しベクトルと衝突しているかチェックを行う
	for (auto& obj : _objs)
	{
		if (CollisionDetector::Instance().CheckColAndVector(obj->Collider(), startPos, direction, length))
		{
			ret.push_back(obj);
		}
	}

	//子ノードに対し再帰的に取得処理を行う
	for (shared_ptr<OcTreeNode> node : _children)
	{
		const auto& child = node->GetByVector(startPos, direction, length);
		ret.insert(ret.end(), child.begin(), child.end());
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
	//渡されていたのが数字の1ではなく文字の'1'であったことが判明したため数字に変換する
	int ret = x - '0';

	return ret == 1 ? ret : -1;
}