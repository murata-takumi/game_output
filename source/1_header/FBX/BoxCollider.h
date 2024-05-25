#pragma once
#include "Application.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class BoxCollider
{
private:
	//��ʂɕ\�����钸�_
	vector<XMVECTOR> _verts;					
	//���_�̏����l
	vector<XMVECTOR> _initVerts;					
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
	void Update(const XMMATRIX& mat);	

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;	
	//���E�l��Ԃ��֐�
	map<string, float> BoundValues()const;
};
