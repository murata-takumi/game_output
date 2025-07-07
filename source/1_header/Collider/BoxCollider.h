#pragma once
#include "Application.h"

#include "Collider/ICollider.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class ColliderComposition;
class IFbx;
class Vector3;
class BoxCollider : public ICollider
{
private:
	//���_�̏����l
	vector<Vector3> _initVerts;
	//��ʂɕ\�����钸�_
	vector<Vector3> _verts;

	//�R���|�W�V�����N���X
	shared_ptr<ColliderComposition> _colliderComp;

	//�����x�N�g��
	Vector3* _directionVecs = new Vector3[3];

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	//�����蔻�肪���O�����̃x�N�g����ݒ肷��֐�
	void SetVec(const Vector3& vec);

public:
	//�R���X�g���N�^
	BoxCollider() = default;
	//�f�X�g���N�^
	~BoxCollider() = default;

	//�������֐�
	void Init(const Vector3& size, const Vector3& pos = Vector3(0, 0, 0),
		IFbx* object = nullptr);

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���_��Ԃ��֐�
	vector<Vector3> Vertices()const;

	Vector3* DirectionVectors()const;

	//���A�����A���s��Ԃ�
	Vector3 HalfLength()const;

	//���S���W��Ԃ�
	shared_ptr<Vector3> Center()const;

	//�I�u�W�F�N�g��Ԃ�
	IFbx& Object()const;
};
