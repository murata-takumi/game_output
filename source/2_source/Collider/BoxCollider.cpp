#pragma once
#include "Collider/BoxCollider.h"
#include "FBX/FBXBase.h"
#include "Includes.h"

//���ʃx�N�g���̏����l
const Vector3 INITIAL_FRONT = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

/// <summary>
/// �R���X�g���N�^
/// �����蔻����\�����钸�_���쐬����
/// </summary>
/// <param name="size">�����蔻��̕��A�����A���s�̃T�C�Y</param>
/// <param name="center">�����蔻��̒��S�x�N�g��</param>
BoxCollider::BoxCollider(FBXBase& object, const Vector3& size)
	:_object(object)
{
	//�x�N�g����������
	SetVec(INITIAL_FRONT);

	auto x = size.X();
	auto y = size.Y();
	auto z = size.Z();

	//�����̒������擾
	_halfWidth = size.X() / 2;
	_halfHeight = size.Y() / 2;
	_halfDepth = size.Z() / 2;

	//���S��������
	_initCenter = XMVectorSet(0,0,0,0);

	//���_�̏�����
	_initVerts.emplace_back(XMVectorSet(-x / 2, -y / 2, -z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(-x / 2, -y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, -y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, -y / 2, -z / 2, 0.0f));

	_initVerts.emplace_back(XMVectorSet(-x / 2, y / 2, -z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(-x / 2, y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, y / 2, -z / 2, 0.0f));

	//���_����
	_center = _initCenter;
	_verts = _initVerts;
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
BoxCollider::SetVec(const Vector3& vec)
{
	//�܂��͐��ʃx�N�g��
	_frontDir = XMVector3Normalize(vec);

	//XZ���ʂɑ΂������ȃx�N�g�����Ƃ�A����Ɛ��ʃx�N�g���̊O�ς����E�x�N�g���Ƃ���
	auto vertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_rightDir = XMVector3Normalize(XMVector3Cross(_frontDir, vertical));

	//���ʃx�N�g���A�E�x�N�g���̊O�ς���x�N�g���Ƃ���
	_upDir = XMVector3Normalize(XMVector3Cross(_frontDir, _rightDir));
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
	_center = XMVector3Transform(_initCenter, mat);

	//�e�������W�ɑ΂����W�ϊ����s���A���ۂɕ\��������W�ɑ��
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], mat);
	}															
}

/// <summary>
/// �����蔻����\�����钸�_�̃x�N�g����Ԃ��֐�
/// </summary>
/// <returns>���_�̃x�N�g��</returns>
vector<Vector3>
BoxCollider::Vertices()const
{
	return _verts;
}

/// <summary>
/// ���A�����A���s���܂Ƃ߂ĕԂ��֐�
/// </summary>
/// <returns>���A�����A���s</returns>
Vector3
BoxCollider::HalfLength()const
{
	return Vector3(_halfWidth,_halfHeight,_halfDepth);
}

/// <summary>
/// ���S���W��Ԃ��֐�
/// </summary>
/// <returns></returns>
Vector3
BoxCollider::Center()const
{
	return _center;
}

/// <summary>
/// �����蔻������I�u�W�F�N�g��Ԃ�
/// </summary>
/// <returns>�I�u�W�F�N�g</returns>
FBXBase&
BoxCollider::Object()const
{
	return _object;
}