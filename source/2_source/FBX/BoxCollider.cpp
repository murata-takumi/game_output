#pragma once
#include "FBX/BoxCollider.h"
#include "Includes.h"

//���ʃx�N�g���̏����l
const XMVECTOR INITIAL_FRONT = XMVectorSet(0.0f,0.0f,1.0f,0.0f);
//���ʂƉE�̊p�x��臒l
const float THRESHOLD_BETWEEN_FRONT_AND_RIGHT = 45.0f;
//�E�Ɣw��̊p�x��臒l
const float THRESHOLD_BETWEEN_RIGHT_AND_BACK = 135.0f;
//�w��ƍ��̊p�x��臒l
const float THRESHOLD_BETWEEN_BACK_AND_LEFT = 225.0f;
//���Ɛ��ʂ̊p�x��臒l
const float THRESHOLD_BETWEEN_LEFT_AND_FRONT = 315.0f;

/// <summary>
/// �R���X�g���N�^
/// �����蔻����\�����钸�_���쐬����
/// </summary>
/// <param name="size">�����蔻��̕��A�����A���s�̃T�C�Y</param>
/// <param name="center">�����蔻��̒��S�x�N�g��</param>
BoxCollider::BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center)
{
	//�x�N�g����������
	SetVec(INITIAL_FRONT);

	auto x = size.x;
	auto y = size.y;
	auto z = size.z;

	//�����̒������擾
	_halfWidth = size.x / 2;
	_halfHeight = size.y / 2;
	_halfDepth = size.z / 2;

	//���S��������
	_initCenter = XMLoadFloat3(&center);
	auto centerX = center.x;
	auto centerY = center.y;
	auto centerZ = center.z;

	//���_�̏�����
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY - y / 2, centerZ - z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY - y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY - y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY - y / 2, centerZ - z / 2, 0.0f));

	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY + y / 2, centerZ - z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY + y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY + y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY + y / 2, centerZ - z / 2, 0.0f));

	//���_����
	_verts = _initVerts;
	_center = _initCenter;																				
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
BoxCollider::~BoxCollider()
{

}

/// <summary>
/// �����蔻�肪���O�����̃x�N�g����ݒ肷��֐�
/// </summary>
/// <param name="vec">�I�u�W�F�N�g�̐��ʃx�N�g��</param>
void 
BoxCollider::SetVec(const XMVECTOR& vec)
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

/// <summary>
/// ���t���[���̍X�V���s���֐�
/// ���W�ϊ����s��
/// </summary>
/// <param name="mat">���W�ϊ��ɗp����s��</param>
void
BoxCollider::Update(const XMMATRIX& mat)
{
	//�X�P�[�����O�A��]�A���s�ړ��������s�񂩂�擾
	XMVECTOR scale, trans, skew;
	XMMatrixDecompose(&scale, &skew, &trans, mat);

	//X������̉�]�𔽓]
	auto q = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XM_PI);
	skew = XMQuaternionMultiply(skew, q);						

	//�X�P�[�����O�A��]�A���s�ړ���������V�K���[���h�s����쐬���A���s�ړ���X�v�f�𔽓]
	XMMATRIX newWorldMat = XMMatrixScalingFromVector(scale) *						
		XMMatrixRotationQuaternion(skew) * XMMatrixTranslationFromVector(trans);
	newWorldMat.r[3].m128_f32[0] *= -1.0f;

	//���K���������ʃx�N�g�����擾
	auto inputVec = XMVectorSet(
		-1 * mat.r[2].m128_f32[0],
		mat.r[2].m128_f32[1],
		mat.r[2].m128_f32[2],
		0.0f
	);
	inputVec = XMVector3Normalize(inputVec);

	//OBB�̃x�N�g�����X�V
	SetVec(inputVec);

	//���S���X�V
	_center = XMVector3Transform(_initCenter, newWorldMat);

	//�e�������W�ɑ΂����W�ϊ����s���A���ۂɕ\��������W�ɑ��
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], newWorldMat);
	}															
}

/// <summary>
/// �����蔻����\�����钸�_�̃x�N�g����Ԃ��֐�
/// </summary>
/// <returns>���_�̃x�N�g��</returns>
vector<XMVECTOR>
BoxCollider::Vertices()const
{
	return _verts;
}

/// <summary>
/// ���A�����A���s���܂Ƃ߂ĕԂ��֐�
/// </summary>
/// <returns>���A�����A���s</returns>
XMFLOAT3
BoxCollider::HalfLength()const
{
	return XMFLOAT3(_halfWidth,_halfHeight,_halfDepth);
}