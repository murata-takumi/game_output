#pragma once
#include "Application.h"

/// <summary>
/// Orinented Bounding Box(�L�����E�{�b�N�X)
/// ���ʁA�E�A��x�N�g���ƕ��A�����A���s�̔����A���S���f�[�^�Ƃ��Ď���
/// </summary>
struct OBB
{
	//���ꂼ�ꒆ�S�A���ʁA�E�A��x�N�g��
	XMVECTOR _center,_frontDir,_rightDir,_upDir;

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	/// <summary>
	/// �x�N�g����ݒ肷��֐�
	/// </summary>
	/// <param name="vec"></param>
	void SetVec(const XMVECTOR& vec)
	{
		//�܂��͐��ʃx�N�g��
		_frontDir = XMVector3Normalize(vec);

		//XZ���ʂɑ΂������ȃx�N�g�����Ƃ�A����Ɛ��ʃx�N�g���̊O�ς����E�x�N�g���Ƃ���
		auto vertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		_rightDir = XMVector3Normalize(XMVector3Cross(_frontDir, vertical));

		//���ʃx�N�g���A�E�x�N�g���̊O�ς���x�N�g���Ƃ���
		_upDir = XMVector3Normalize(XMVector3Cross(_frontDir, _rightDir));
		//���]���Y�ꂸ��
		_upDir *= -1.0f;
	}
};

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

	//�I�u�W�F�N�g�̊p�x
	float _angle;

	//���E�l��ݒ肷��֐�
	void SetBoundValues();								

public:
	//�����蔻��
	OBB _obb;

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
