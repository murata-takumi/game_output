#pragma once
#include "Application.h"

#include "Collider/CollisionDetector.h"
#include "Collider/BoxCollider.h"
#include "FBX/FBXObject.h"
#include "OcTree/Bounds.h"

/// <summary>
/// �����؂��\������m�[�h
/// </summary>
class OcTreeNode
{
public:
	//�R���X�g���N�^
	OcTreeNode(Bounds* bounds, int capacity);
	//�f�X�g���N�^
	~OcTreeNode();

	//�I�u�W�F�N�g��ǉ�����֐�
	bool AddObject(const shared_ptr<FBXObject> obj);

	//��Ԃ𕪊�����֐�
	void SubDivide();

	//�q�m�[�h�ɕ�������֐�
	void AddToChild(vector<shared_ptr<FBXObject>> objs);

	//�N�G���͈͂ɂ���I�u�W�F�N�g���擾����֐�
	vector<shared_ptr<FBXObject>> Get(BoxCollider bounds);
private:
	//�Ǘ�������
	Bounds* _bounds;

	//�q�m�[�h
	vector<OcTreeNode> _children;

	//�i�[����I�u�W�F�N�g
	vector<shared_ptr<FBXObject>> _objs;

	//�e��
	int _capacity;

	//1�Ȃ炻�̂܂܁A0�Ȃ�-1��Ԃ��֐�
	int Convert(int x);
};