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
	//���E�l
	map<string,float> _boundValues;
	//���S�x�N�g���̏����l
	XMVECTOR _initCenter;

	//���ʃx�N�g��
	XMVECTOR _frontDir;
	//���x�N�g��
	XMVECTOR _rightDir;
	//��x�N�g��
	XMVECTOR _upDir;
	//�I�u�W�F�N�g�̊p�x
	float _angle;

	//���E�l��ݒ肷��֐�
	void SetBoundValues();								

public:

	//�X�V����钆�S�x�N�g��
	XMVECTOR _center;

	//�R���X�g���N�^
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat,float angle = 0.0f);
	
	//������ݒ肷��֐�
	void SetDir(const XMVECTOR& dir);

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;	
	//���E�l��Ԃ��֐�
	map<string, float> BoundValues()const;
};
