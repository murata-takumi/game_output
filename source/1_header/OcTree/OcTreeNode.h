#pragma once
#include "Application.h"

/// <summary>
/// �����؂��\������m�[�h
/// </summary>
class BoxCollider;
class IFbx;
class Vector3;
class OcTreeNode
{
public:
	//�R���X�g���N�^
	OcTreeNode(const shared_ptr<BoxCollider> col, int capacity);
	//�f�X�g���N�^
	~OcTreeNode();

	//�I�u�W�F�N�g��ǉ�����֐�
	bool AddObject(const shared_ptr<IFbx> obj);

	//��Ԃ𕪊�����֐�
	void SubDivide();

	//�q�m�[�h�ɕ�������֐�
	void AddToChild(const vector<shared_ptr<IFbx>> objs);

	//�N�G���͈͂ɂ���I�u�W�F�N�g���擾����֐�
	vector<shared_ptr<IFbx>> Get(const shared_ptr<BoxCollider> bounds)noexcept;
private:
	//�Ǘ�������
	shared_ptr<BoxCollider> _col;

	//�q�m�[�h
	vector<shared_ptr<OcTreeNode>> _children;

	//�i�[����I�u�W�F�N�g
	vector<shared_ptr<IFbx>> _objs;

	//�e��
	int _capacity;

	//1�Ȃ炻�̂܂܁A0�Ȃ�-1��Ԃ��֐�
	int Convert(int x);
};