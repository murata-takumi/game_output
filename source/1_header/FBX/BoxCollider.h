#pragma once
#include "Application.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class BoxCollider
{
private:
	//���_�̏����l
	vector<XMVECTOR> _initVerts;
	//��ʂɕ\�����钸�_
	vector<XMVECTOR> _verts;
	//���S�x�N�g���̏����l
	XMVECTOR _initCenter;

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	//�����蔻�肪���O�����̃x�N�g����ݒ肷��֐�
	void SetVec(const XMVECTOR& vec);

public:
	//���ꂼ�ꒆ�S�A���ʁA�E�A��x�N�g��
	XMVECTOR _center, _frontDir, _rightDir, _upDir;

	//�R���X�g���N�^
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;

	XMFLOAT3 HalfLength()const;
};
