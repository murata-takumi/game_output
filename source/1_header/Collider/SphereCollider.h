#pragma once
#include "Application.h"

#include "Collider/ICollider.h"

/// <summary>
/// ����̓����蔻�����������N���X
/// </summary>
class ColliderComposition;
class IFbx;
class Vector3;
class SphereCollider : public ICollider
{
private:
	//�R���|�W�V�����N���X
	shared_ptr<ColliderComposition> _colliderComp;

	//���a
	float _radius;

public:
	SphereCollider() = default;
	~SphereCollider() = default;

	//�������֐�
	void Init(float radius, const Vector3& pos = Vector3(0, 0, 0),
		IFbx* object = nullptr);

	//���W���X�V����֐�
	void Update(const XMMATRIX & mat);

	//���S���W��Ԃ�
	shared_ptr<Vector3> Center()const;

	//�I�u�W�F�N�g��Ԃ�
	IFbx& Object()const;

	//���a
	float Radius()const;
};