#pragma once
#include "Application.h"

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
	vector<XMVECTOR> _initVerts;
	//��ʂɕ\�����钸�_
	vector<XMVECTOR> _verts;
	//���S�x�N�g���̏����l
	XMVECTOR _initCenter;
	//���S�x�N�g��
	XMVECTOR _center;

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	//�����蔻�肪���O�����̃x�N�g����ݒ肷��֐�
	void SetVec(const XMVECTOR& vec);

public:
	//���ꂼ�ꒆ�S�A���ʁA�E�A��x�N�g��
	XMVECTOR _frontDir, _rightDir, _upDir;

	//�R���X�g���N�^
	BoxCollider(FBXBase& object, const XMFLOAT3& size);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;

	//���A�����A���s��Ԃ�
	XMFLOAT3 HalfLength()const;

	//���S���W��Ԃ�
	XMVECTOR Center()const;

	//�I�u�W�F�N�g��Ԃ�
	FBXBase& Object()const;
};
