#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "FBX/FBXBase.h"
#include "OcTree/OcTree.h"
#include "OcTree/OcTreeNode.h"

/// <summary>
/// �V���O���g����Ԃ�
/// </summary>
/// <returns>�V���O���g��</returns>
OcTree&
OcTree::Instance()
{
	static OcTree instance;
	return instance;
}

/// <summary>
/// �R���X�g���N�^�ƃf�X�g���N�^
/// �ǂ������g��Ȃ�
/// </summary>
OcTree::OcTree()
{

}
OcTree::~OcTree()
{

}

/// <summary>
/// �������֐�
/// </summary>
/// <param name="col">�Ǘ�������</param>
/// <param name="capacity">�m�[�h�����Ă�ő�e��</param>
void
OcTree::Init(const shared_ptr<BoxCollider> col, int capacity)
{
	//���[�g�m�[�h��������
	_rootNode = make_shared<OcTreeNode>(col, capacity);
}

/// <summary>
/// �I�u�W�F�N�g��ǉ�����֐�
/// </summary>
/// <param name="obj">�ǉ�����I�u�W�F�N�g</param>
/// <returns>�ǉ��ŗ������ǂ���</returns>
bool 
OcTree::AddObject(const shared_ptr<FBXBase> obj)
{
	//���[�g�m�[�h�ɒǉ�����
	return _rootNode->AddObject(obj);
}

/// <summary>
/// �N�G���͈͂ɂ���I�u�W�F�N�g���擾����֐�
/// </summary>
/// <param name="bounds">�N�G���͈�</param>
/// <returns>�擾�����I�u�W�F�N�g</returns>
vector<shared_ptr<FBXBase>>
OcTree::Get(const shared_ptr<BoxCollider> col)noexcept
{
	return _rootNode->Get(col);
}