#pragma once
#include "Application.h"

/// <summary>
/// �����؂��Ǘ�����N���X
/// </summary>
class BoxCollider;
class FBXBase;
class OcTreeNode;
class OcTree
{
public:
	static OcTree& Instance();

	//�������֐�
	void Init(const shared_ptr<BoxCollider> col, int capacity);

	//�I�u�W�F�N�g��ǉ�����֐�
	bool AddObject(const shared_ptr<FBXBase> obj);

	//�N�G���͈͂ɂ���I�u�W�F�N�g���擾����֐�
	vector<shared_ptr<FBXBase>> Get(const shared_ptr<BoxCollider> col)noexcept;

private:
	//�m�[�h
	shared_ptr<OcTreeNode> _rootNode;

	//�R���X�g���N�^�A�f�X�g���N�^
	OcTree();
	OcTree(const OcTree&) = delete;
	~OcTree();
};