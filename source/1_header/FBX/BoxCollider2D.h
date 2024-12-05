#pragma once
#include "Application.h"

/// <summary>
/// ��`�̓����蔻�����������N���X
/// </summary>
class BoxCollider2D
{
private:
	//���S�x�N�g���̏����l
	XMVECTOR _initCenter;

	RECT _rect;

	//���ꂼ�ꒆ�S�A�E�A��x�N�g��
	XMVECTOR _center, _rightDir, _upDir;

public:
	//���ꂼ�ꕝ�A�����A���s�̒����̔���
	float _halfWidth, _halfHeight;

	//�R���X�g���N�^
	BoxCollider2D(const XMVECTOR& size, const XMVECTOR& center);
	//�f�X�g���N�^
	~BoxCollider2D();

	//���W���X�V����֐�
	void Update();

	//��`��Ԃ�
	RECT Rect();

	//������̓K�������W��Ԃ�
	XMFLOAT2 StrCenterPos(string& str);
};