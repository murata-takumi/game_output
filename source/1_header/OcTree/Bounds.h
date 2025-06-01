#pragma once
#include "Application.h"
#include "Vector3.h"

#include "Manager/ImGuiManager.h"

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
	Bounds(const Vector3& pos, const Vector3& halfLength);

	//���W����`�̒��ɂ��邩���肷��֐�
	bool CheckPointInBounds(const Vector3& point);

	//���S���W��Ԃ�
	Vector3& Pos();

	//����������Ԃ�
	Vector3 HalfLength()const;
};
