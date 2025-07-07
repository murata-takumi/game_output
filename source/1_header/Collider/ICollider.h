#pragma once
#include "Application.h"

/// <summary>
/// �����蔻��̔h�����ƂȂ�C���^�[�t�F�[�X
/// </summary>
class IFbx;
class Vector3;
class ICollider
{
private:

public:
	//�R���X�g���N�^�A�f�X�g���N�^
	ICollider() = default;
	~ICollider() = default;

	//�X�V����
	virtual void Update(const XMMATRIX& mat) = 0;

	//���S���W��Ԃ�
	virtual shared_ptr<Vector3> Center()const = 0;

	//�I�u�W�F�N�g��Ԃ�
	virtual IFbx& Object()const = 0;
};