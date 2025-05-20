#include "Collider/BoxCollider2D.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="size">�����蔻��̃T�C�Y</param>
/// <param name="center">���S���W</param>
BoxCollider2D::BoxCollider2D(const XMVECTOR& size, const XMVECTOR& center)
{
	_center = center;

	_halfWidth = size.m128_f32[0] / 2;
	_halfHeight = size.m128_f32[1] / 2;

	_rightDir = XMVectorSet(_halfWidth,0.0f,0.0f,0.0f);
	_upDir = XMVectorSet(0.0f,_halfHeight,0.0f, 0.0f);

	_rect = 
	{
		(LONG)(center.m128_f32[0] - size.m128_f32[0] / 2),
		(LONG)(center.m128_f32[1] - size.m128_f32[1] / 2),
		(LONG)(center.m128_f32[0] + size.m128_f32[0] / 2),
		(LONG)(center.m128_f32[1] + size.m128_f32[1] / 2),
	};
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
BoxCollider2D::~BoxCollider2D()
{

}

/// <summary>
/// ��`��Ԃ�
/// </summary>
/// <returns>��`</returns>
RECT
BoxCollider2D::Rect()
{
	return _rect;
}

/// <summary>
/// ������𒆉������ɂ���
/// </summary>
/// <param name="str">������</param>
/// <returns>���������ɂȂ�悤�ȍ��W</returns>
XMFLOAT2
BoxCollider2D::StrCenterPos(string& str)
{
	//�������������ɂ���
	if (str.length() % 2 != 0)
	{
		str = " " + str;
	}

	//��`�̒��S���當�����~15��������
	return XMFLOAT2(Rect().left + _halfWidth - str.length() * 15, Rect().top - 5);
}