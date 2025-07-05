#pragma once
#include "Includes.h"
#include "Vector3.h"

#include "Collider/SphereCollider.h"
#include "Collider/ColliderComposition.h"
#include "FBX/IFbx.h"

/// <summary>
/// �������֐�
/// </summary>
/// <param name="radius">���a</param>
/// <param name="pos">�������W</param>
/// <param name="object">�R�Â���I�u�W�F�N�g</param>
void 
SphereCollider::Init(float radius, const Vector3& pos, IFbx* object)
{
	//���a��ݒ�
	_radius = radius;

	//�R���|�W�V������������
	_colliderComp = make_shared<ColliderComposition>(pos, object);
}

/// <summary>
/// ���t���[���̍X�V���s���֐�
/// ���W�ϊ����s��
/// </summary>
/// <param name="mat">���W�ϊ��ɗp����s��</param>
void
SphereCollider::Update(const XMMATRIX& mat)
{
	//���S���W���X�V
	_colliderComp->Update(mat);
}

/// <summary>
/// ���S���W��Ԃ��֐�
/// </summary>
/// <returns>�|�C���^</returns>
shared_ptr<Vector3>
SphereCollider::Center()const
{
	return _colliderComp->Center();
}

/// <summary>
/// �����蔻������I�u�W�F�N�g��Ԃ�
/// </summary>
/// <returns>�I�u�W�F�N�g</returns>
IFbx&
SphereCollider::Object()const
{
	return _colliderComp->Object();
}

float 
SphereCollider::Radius()const
{
	return _radius;
}