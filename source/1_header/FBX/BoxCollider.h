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

public:

	//���ꂼ�ꒆ�S�A���ʁA�E�A��x�N�g��
	XMVECTOR _center, _frontDir, _rightDir, _upDir;

	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight, _halfDepth;

	//�R���X�g���N�^
	BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center);	
	//�f�X�g���N�^
	~BoxCollider();

	//���W���X�V����֐�
	void Update(const XMMATRIX& mat);

	//���_��Ԃ��֐�
	vector<XMVECTOR> Vertices()const;
};
