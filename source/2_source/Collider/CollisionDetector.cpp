#include "Collider/CollisionDetector.h"

/// <summary>
/// �V���O���g����Ԃ�
/// </summary>
/// <returns>�V���O���g��</returns>
CollisionDetector&
CollisionDetector::Instance()
{
	static CollisionDetector instance;
	return instance;
}

/// <summary>
/// �������藝���OBB���Փ˂��Ă��邩�m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩�ǂ���</returns>
bool
CollisionDetector::CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2)
{
	return CheckOBBIntersection(col1, col2) && CheckOBBIntersection(col2, col1);
}

/// <summary>
/// OBB�ɐ����������Ă��邩�m�F����֐�
/// </summary>
/// <param name="col">OBB</param>
/// <param name="startPos">�����̎n�_</param>
/// <param name="endPos">�����̏I�_</param>
/// <returns></returns>
bool
CollisionDetector::CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& endPos)
{
	//�����̒��S���擾
	Vector3 lineCenter = (startPos + endPos) / 2;
	//�����̌������擾
	Vector3 lineDir = endPos - startPos;
	//�����̔����̒���
	float lineExtent = XMVector3Length(lineDir).m128_f32[0] * 0.5f;

	//�������Z�������ꍇ�͓_�Ƃ��Ĉ���
	if (lineExtent < FLT_EPSILON)
	{
		//�P�ʃx�N�g��
		lineDir = Vector3(1, 0, 0);
		lineExtent = 0.0f;
	}
	else
	{
		lineDir = XMVector3Normalize(endPos - startPos);
	}

	//OBB�Ɛ����̒��S�̍���
	Vector3 centerDiff = lineCenter - col.Center();

	float r, r0, r1;
	for (int i = 0; i < 3; i++)
	{
		//r = abs(XMVector3Dot(centerDiff,col.))
	}

	return true;
}

/// <summary>
/// OBB�ɍ��W�����荞��ł��邩�m�F����֐�
/// </summary>
/// <param name="col">�Ώۂ�OBB</param>
/// <param name="point">���W</param>
/// <returns>���荞��ł��邩�ǂ���</returns>
bool
CollisionDetector::CheckColAndPoint(const BoxCollider& col, const Vector3& point)
{
	//���W��OBB�̒��S�����ԃx�N�g�����擾
	auto vecBetcolAndPoint = col.Center() - point;

	//�����x�N�g������͂ݏo�Ă���x�N�g��
	Vector3 vec = XMVectorSet(0, 0, 0, 0);

	for (int i = 0; i < 3; i++)
	{
		//�����x�N�g���ɑ΂���䗦���擾���A��Βl��1���傫����΁i=�͂ݏo�Ă�����j�����ɉ��Z
		float s = XMVector3Dot(vecBetcolAndPoint, col.DirectionVectors()[i]).m128_f32[0] / col.HalfLength()[i];
		if (fabs(s) > 1)
		{
			vec += (1 - fabs(s)) * col.HalfLength()[i] * col.DirectionVectors()[i];
		}
	}

	//������0�ȉ���������OBB�̒��ɍ��W�����荞��ł���
	if (XMVector3Length(vec).m128_f32[0] <= 0)
	{
		return true;
	}

	return false;
}

bool 
CollisionDetector::CheckOBBIntersection(const BoxCollider& col1, const BoxCollider& col2)
{
	//�eOBB�̉E�A��A���ʃx�N�g��
	Vector3 center1 = col1.Center();
	Vector3 center2 = col2.Center();

	//�o����OBB�̒��S�����ԃx�N�g��
	Vector3 vecBetCenter = center2 - center1;

	for (int i = 0; i < 3; i++)
	{
		//�e�����x�N�g���̕������ւ̓��e�̘a
		float r = 0;
		for (int j = 0; j < 3; j++)
		{
			r += LenOnSeparateAxis(col1.DirectionVectors()[i], col2.DirectionVectors()[j] * col2.HalfLength()[j]);
		}

		//���S�ԃx�N�g���̕������ւ̓��e
		float s = XMVector3Dot(vecBetCenter, col1.DirectionVectors()[i]).m128_f32[0];

		//��r���āA���S�ԃx�N�g���̓��e�̕����傫����ΏՓ˂��Ă��Ȃ�
		if (fabs(s) > r + col1.HalfLength()[i])
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// �������ɑ΂����e���ꂽ�x�N�g���̑傫�������߂�֐�
/// </summary>
/// <param name="sep">������</param>
/// <param name="dirVec">�����x�N�g��</param>
/// <returns>��������̕����x�N�g���̑傫��</returns>
float
CollisionDetector::LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec)
{
	//�������Ɗe�x�N�g���̓��ς����A���̐�Βl�����e���ꂽ�傫���ƂȂ�
	float ret = fabs(XMVector3Dot(sep, dirVec).m128_f32[0]);

	return ret;
}