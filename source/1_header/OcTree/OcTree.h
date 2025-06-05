#pragma once
#include "Application.h"

#include "Collider/BoxCollider.h"
#include "OcTree/OcTreeNode.h"

/// <summary>
/// �����؂��Ǘ�����N���X
/// </summary>
class OcTree
{
public:
	static OcTree& Instance();

	//�������֐�
	void Init(const shared_ptr<BoxCollider> col, int capacity);

	//�I�u�W�F�N�g��ǉ�����֐�
	bool AddObject(const shared_ptr<FBXObject> obj);

	//�N�G���͈͂ɂ���I�u�W�F�N�g���擾����֐�
	vector<shared_ptr<FBXObject>> Get(const shared_ptr<BoxCollider> col);

private:
	//�m�[�h
	shared_ptr<OcTreeNode> _rootNode;

	//�R���X�g���N�^�A�f�X�g���N�^
	OcTree();
	OcTree(const OcTree&) = delete;
	~OcTree();
};