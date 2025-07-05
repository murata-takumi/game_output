#pragma once
#include "Application.h"

class IFbx;
class Vector3;
class ColliderComposition
{
private:
	//���̓����蔻������I�u�W�F�N�g
	shared_ptr<IFbx> _object;

	//���S�x�N�g���̏����l
	shared_ptr <Vector3> _initCenter;
	//���S�x�N�g��
	shared_ptr <Vector3> _center;

public:
	//�R���X�g���N�^
	ColliderComposition(const Vector3& pos = Vector3(0, 0, 0),IFbx* object = nullptr);

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���S���W��Ԃ�
	shared_ptr <Vector3> Center()const;

	//�I�u�W�F�N�g��Ԃ�
	IFbx& Object()const;
};