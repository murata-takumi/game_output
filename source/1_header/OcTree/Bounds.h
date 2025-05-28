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
	Vector3 _pos;

	//���A�����A���s
	Vector3 _halfLength;

public:

	//�R���X�g���N�^
	Bounds(Vector3 pos, Vector3 halfLength);

	//���W����`�̒��ɂ��邩���肷��֐�
	bool CheckPointInBounds(Vector3& point);

	//���S���W��Ԃ�
	Vector3 Pos()const;

	//����������Ԃ�
	Vector3 HalfLength()const;
};
