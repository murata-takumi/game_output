#include "OcTree/Bounds.h"

/// <summary>
/// �R���X�g���N�^
/// �e��ϐ���������
/// </summary>
/// <param name="pos">���W</param>
/// <param name="halfLength">�e���W���̔����̒���</param>
Bounds::Bounds(XMFLOAT3 pos, XMFLOAT3 halfLength) 
	:_pos(pos), _halfLength(halfLength)
{

}

/// <summary>
/// ���W����`�̒��ɂ��邩���肷��֐�
/// </summary>
/// <param name="point">���W</param>
/// <returns>���ɂ��邩</returns>
bool 
Bounds::CheckPointInBounds(XMFLOAT3 point)
{
	//�܂��͍��W�����ԃx�N�g�����擾
	auto a = XMFLOAT3(
		point.x - _pos.x,
		point.y - _pos.y,
		point.z - _pos.z
	);
	Vector3 vecBetPos = XMLoadFloat3(&a);

	//�����̒�����XMVECTOR�ɕϊ�
	Vector3 halfLengthVec = XMLoadFloat3(&_halfLength);

	//�x�N�g���̊e�v�f�ɑ΂��������s��
	for (int i = 0; i < 3; i++)
	{
		//�e���W���ɑ΂���䗦�����߂�i�����̒����s�b�^�����������Βl��1�ɂȂ�j
		auto s = XMVector3Dot(vecBetPos, XMMatrixIdentity().r[i]) / halfLengthVec[i];
		//�䗦�̐�Βl��1��肨��������΂͂ݏo�Ă���Ƃ݂Ȃ��A���Ɋ܂�ł��Ȃ�
		if (fabs(s.m128_f32[0]) > 1)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// ���W����`�̒��ɂ��邩���肷��֐��iXMVector�Łj
/// </summary>
/// <param name="vec">���W�x�N�g��</param>
/// <returns>���ɂ��邩</returns>
bool 
Bounds::CheckPointInBounds(Vector3& vec)
{
	//XMVector����XMFLOAT3�ɕϊ����������œn��
	XMFLOAT3 temp = XMFLOAT3(vec.X(), vec.Y(), vec.Z());

	return CheckPointInBounds(temp);
}

/// <summary>
/// ���S���W��Ԃ�
/// </summary>
/// <returns></returns>
XMFLOAT3 
Bounds::Pos()const
{
	return _pos;
}

/// <summary>
/// ����������Ԃ�
/// </summary>
/// <returns></returns>
XMFLOAT3 
Bounds::HalfLength()const
{
	return _halfLength;
}