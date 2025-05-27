#pragma once
#include "Application.h"
#include "Vector3.h"

/// <summary>
/// 3D��Ԃɂ������`��\���N���X
/// </summary>
class Bounds
{
private:
	//���S���W
	XMFLOAT3 _pos;

	//���A�����A���s
	XMFLOAT3 _halfLength;

public:

	//�R���X�g���N�^
	Bounds(XMFLOAT3 pos, XMFLOAT3 halfLength);

	//���W����`�̒��ɂ��邩���肷��֐�
	bool CheckPointInBounds(XMFLOAT3 point);

	//���W����`�̒��ɂ��邩���肷��֐��iXMVector�Łj
	bool CheckPointInBounds(Vector3& vec);

	//���S���W��Ԃ�
	XMFLOAT3 Pos()const;

	//����������Ԃ�
	XMFLOAT3 HalfLength()const;
};
