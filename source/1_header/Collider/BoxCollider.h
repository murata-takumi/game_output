#pragma once
#include "Application.h"
#include "Vector3.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class FBXBase;
class BoxCollider
{
private:
	//���̓����蔻������I�u�W�F�N�g
	FBXBase& _object;

	//���_�̏����l
	vector<Vector3> _initVerts;
	//��ʂɕ\�����钸�_
	vector<Vector3> _verts;
	//���S�x�N�g���̏����l
	Vector3 _initCenter;
	//���S�x�N�g��
	Vector3 _center;

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	//�����蔻�肪���O�����̃x�N�g����ݒ肷��֐�
	void SetVec(const Vector3& vec);

public:
	//���ꂼ�ꒆ�S�A���ʁA�E�A��x�N�g��
	Vector3 _frontDir, _rightDir, _upDir;

	//�R���X�g���N�^
	BoxCollider(FBXBase& object, const Vector3& size);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���_��Ԃ��֐�
	vector<Vector3> Vertices()const;

	//���A�����A���s��Ԃ�
	Vector3 HalfLength()const;

	//���S���W��Ԃ�
	Vector3 Center()const;

	//�I�u�W�F�N�g��Ԃ�
	FBXBase& Object()const;
};
