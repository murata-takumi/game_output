#include "OcTree/OcTreeNode.h"

#include <bitset>

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="bounds">�������</param>
/// <param name="capacity">�e��</param>
OcTreeNode::OcTreeNode(const shared_ptr<BoxCollider> col, int capacity):
	_col(col),_capacity(capacity)
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
OcTreeNode::~OcTreeNode()
{

}

/// <summary>
/// �I�u�W�F�N�g��ǉ�����֐�
/// </summary>
/// <param name="obj">�ǉ�����I�u�W�F�N�g</param>
/// <returns></returns>
bool
OcTreeNode::AddObject(const shared_ptr<FBXObject> obj)
{
	//��ԓ��ɓ����ĂȂ������珈�����f
	if (!CollisionDetector::Instance().CheckColAndPoint(*_col, obj.get()->Pos()))
	{
		assert(0);
		return false;
	}

	//�x�N�g���ɃI�u�W�F�N�g�ǉ�
	_objs.push_back(obj);

	//�I�u�W�F�N�g�����e�ʂ���������q�m�[�h�쐬�A���z
	if (_objs.size() > _capacity)
	{
		SubDivide();

		AddToChild(_objs);

		//���z������x�N�g���͏�����
		_objs.clear();
	}

	return true;
}

/// <summary>
/// �Ǘ������Ԃ�8�ɕ������A���������q�m�[�h�ɊǗ�������֐�
/// </summary>
void
OcTreeNode::SubDivide()
{
	// ��Ԃ̔����������擾���A�X�ɔ����ɂ���
	auto bh = _col->HalfLength();
	Vector3 halfVec = XMVectorScale(bh, 1.0f / 2.0f);

	//�V������`�̍��W�Ɏg�p
	float quaterLength = halfVec.X() / 2.0f;
	for (int i = 0; i < 8; i++)
	{
		//i���r�b�g�ɕϊ����A�e����x,y,z���W�Ɏg�p
		std::bitset<3> decimal(i);
		string decStr = decimal.to_string();

		int x = Convert(decStr[0]) * quaterLength;
		int y = Convert(decStr[1]) * quaterLength;
		int z = Convert(decStr[2]) * quaterLength;

		//�V������`���쐬
		shared_ptr<BoxCollider> newCol = make_shared<BoxCollider>(Vector3(halfVec), Vector3(x, y, z));

		//�q�m�[�h���쐬���A�x�N�g���Ɋi�[
		_children.push_back(make_shared<OcTreeNode>(newCol,_capacity));
	}
}

/// <summary>
/// �q�m�[�h�ɃI�u�W�F�N�g��ǉ�����֐�
/// </summary>
/// <param name="objs">�I�u�W�F�N�g</param>
void
OcTreeNode::AddToChild(const vector<shared_ptr<FBXObject>> objs)
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
/// �N�G���͈͓��Ɋ܂܂�Ă���I�u�W�F�N�g��S�Ď擾����֐�
/// </summary>
/// <param name="bounds">�N�G���͈�</param>
/// <returns>�I�u�W�F�N�g</returns>
vector<shared_ptr<FBXObject>>
OcTreeNode::Get(const shared_ptr<BoxCollider> col)
{
	vector<shared_ptr<FBXObject>> ret;

	//�S�I�u�W�F�N�g�ɑ΂��N�G���͈͓��Ɋ܂܂�Ă��邩�`�F�b�N���s��
	for (auto& obj : _objs)
	{
		if (CollisionDetector::Instance().CheckColAndCol(*col,*obj->Collider()))
		{
			ret.push_back(obj);
		}
	}

	//�q�m�[�h�ɑ΂��ċA�I�Ɏ擾�������s��
	for (shared_ptr<OcTreeNode> node : _children)
	{
		ret.insert(ret.end(), node->Get(col).begin(), node->Get(col).end());
	}

	return ret;
}

/// <summary>
/// 1�Ȃ炻�̂܂܁A0�Ȃ�-1�ɕϊ�����֐�
/// </summary>
/// <param name="x">1or0</param>
/// <returns>1or-1</returns>
int
OcTreeNode::Convert(int x)
{
	return x == 1 ? x : -1;
}