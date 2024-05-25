#include "FBX/BoxCollider.h"
#include "Constant.h"

/// <summary>
/// �R���X�g���N�^
/// �����蔻����\�����钸�_���쐬����
/// </summary>
/// <param name="x">X��</param>
/// <param name="y">Y����</param>
/// <param name="z">Z��</param>
/// <param name="center">�����蔻��̒��S�x�N�g��</param>
BoxCollider::BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center)
{
	auto x = size.x;
	auto y = size.y;
	auto z = size.z;

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

	//���E�l��������
	SetBoundValues();																					
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
BoxCollider::~BoxCollider()
{

}

/// <summary>
/// �����蔻��ɕK�v�ȋ��E�l��ݒ肷��֐�
/// </summary>
void
BoxCollider::SetBoundValues()
{
	//X���W�̋��E�l
	_boundValues[Constant::BOX_COL_X_MIN] = _verts[0].m128_f32[0];
	_boundValues[Constant::BOX_COL_X_MAX] = _verts[2].m128_f32[0];
	//Y���W�̋��E�l
	_boundValues[Constant::BOX_COL_Y_MIN] = _verts[0].m128_f32[1];
	_boundValues[Constant::BOX_COL_Y_MAX] = _verts[4].m128_f32[1];
	//Z���W�̋��E�l
	_boundValues[Constant::BOX_COL_Z_MIN] = _verts[0].m128_f32[2];
	_boundValues[Constant::BOX_COL_Z_MAX] = _verts[1].m128_f32[2];
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

	//�e�������W�ɑ΂����W�ϊ����s���A���ۂɕ\��������W�ɑ��
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], newWorldMat);
	}

	//���E�l���X�V
	SetBoundValues();																
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
/// �����蔻��̊e���W���擾����֐�
/// </summary>
/// <returns>�e���W�̃x�N�g��</returns>
map<string, float>
BoxCollider::BoundValues()const
{
	return _boundValues;
}