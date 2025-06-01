#include "OcTree/Bounds.h"

/// <summary>
/// �R���X�g���N�^
/// �e��ϐ���������
/// </summary>
/// <param name="pos">���W</param>
/// <param name="halfLength">�e���W���̔����̒���</param>
Bounds::Bounds(const Vector3& pos, const Vector3& halfLength) 
	:_pos(pos), _halfLength(halfLength)
{

}

/// <summary>
/// ���W����`�̒��ɂ��邩���肷��֐�
/// </summary>
/// <param name="point">���W</param>
/// <returns>���ɂ��邩</returns>
bool 
Bounds::CheckPointInBounds(const Vector3& point)
{
	//�܂��͍��W�����ԃx�N�g�����擾
	auto a = Vector3(
		point.X() - _pos.X(),
		point.Y() - _pos.Y(),
		point.Z() - _pos.Z()
	);
	Vector3 vecBetPos = a;

	//�����̒�����XMVECTOR�ɕϊ�
	Vector3 halfLengthVec = _halfLength;

	//�x�N�g���̊e�v�f�ɑ΂��������s��
	for (int i = 0; i < 3; i++)
	{
		//�e���W���ɑ΂���䗦�����߂�i�����̒����s�b�^�����������Βl��1�ɂȂ�j
		auto s = XMVector3Dot(vecBetPos, XMMatrixIdentity().r[i]).m128_f32[0] / halfLengthVec[i];
		//�䗦�̐�Βl��1��肨��������΂͂ݏo�Ă���Ƃ݂Ȃ��A���Ɋ܂�ł��Ȃ�
		if (fabs(s) > 1)
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// ���S���W��Ԃ�
/// </summary>
/// <returns></returns>
Vector3&
Bounds::Pos()
{
	return _pos;
}

/// <summary>
/// ����������Ԃ�
/// </summary>
/// <returns></returns>
Vector3
Bounds::HalfLength()const
{
	return _halfLength;
}