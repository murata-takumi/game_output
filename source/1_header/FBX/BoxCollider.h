#pragma once
#include "Application.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class BoxCollider
{
private:
	//�I�u�W�F�N�g�̊p�x
	float _angle;

	//���_�̏����l
	vector<XMVECTOR> _initVerts;
	//��ʂɕ\�����钸�_
	vector<XMVECTOR> _verts;
	//���E�l
	map<string,float> _boundValues;

	//���E�l��ݒ肷��֐�
	void SetBoundValues();								

public:

	//�R���X�g���N�^
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat,float angle = 0.0f);

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;	
	//���E�l��Ԃ��֐�
	map<string, float> BoundValues()const;
};
