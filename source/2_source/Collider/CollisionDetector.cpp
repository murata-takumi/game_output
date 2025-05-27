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
/// ���OBB���Փ˂��Ă��邩�m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩�ǂ���</returns>
bool
CollisionDetector::CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2)
{
	//�eOBB�̉E�A��A���ʃx�N�g��
	Vector3 center1 = col1.Center();
	Vector3 center2 = col2.Center();
	Vector3 rightLen1 = col1._rightDir * col1.HalfLength().x;
	Vector3 upLen1 = col1._upDir * col1.HalfLength().y;
	Vector3 frontLen1 = col1._frontDir * col1.HalfLength().z;
	Vector3 rightLen2 = col2._rightDir * col2.HalfLength().x;
	Vector3 upLen2 = col2._upDir * col2.HalfLength().y;
	Vector3 frontLen2 = col2._frontDir * col2.HalfLength().z;

	//�o����OBB�̒��S�����ԃx�N�g��
	Vector3 vecBetCenter = center2 - center1;

	//obb1_front
	float r = LenOnSeparateAxis(col1._frontDir, rightLen2, upLen2, frontLen2);
	float s = XMVector3Dot(vecBetCenter, col1._frontDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().z)
	{
		return false;
	}

	auto diffZ = r + col1.HalfLength().z - fabs(s);
	if (s <= 0)
	{
		diffZ *= -1.0f;
	}

	//obb1_right
	r = LenOnSeparateAxis(col1._rightDir, rightLen2, upLen2, frontLen2);
	s = XMVector3Dot(vecBetCenter, col1._rightDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().x)
	{
		return false;
	}

	auto diffX = r + col1.HalfLength().x - fabs(s);
	if (s <= 0)
	{
		diffX *= -1.0f;
	}

	//obb1_up
	r = LenOnSeparateAxis(col1._upDir, rightLen2, upLen2, frontLen2);
	s = XMVector3Dot(vecBetCenter, col1._upDir).m128_f32[0];
	if (fabs(s) > r + col1.HalfLength().y)
	{
		return false;
	}

	auto diffY = r + col1.HalfLength().y - fabs(s);

	//obb2_front
	r = LenOnSeparateAxis(col2._frontDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._frontDir).m128_f32[0]);
	if (s > r + col2.HalfLength().z)
	{
		return false;
	}

	//obb2_right
	r = LenOnSeparateAxis(col2._rightDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._rightDir).m128_f32[0]);
	if (s > r + col2.HalfLength().x)
	{
		return false;
	}

	//obb2_up
	r = LenOnSeparateAxis(col2._upDir, rightLen1, upLen1, frontLen1);
	s = fabs(XMVector3Dot(vecBetCenter, col2._upDir).m128_f32[0]);
	if (s > r + col2.HalfLength().y)
	{
		return false;
	}

	//�ŏ��l���X�V
	auto min = fabs(diffX);
	if (min > fabs(diffY))
	{
		min = fabs(diffY);
	}
	if (min > fabs(diffZ))
	{
		min = fabs(diffZ);
	}

	//�ŏ��l�̍��W���ɑ΂��߂荞�݂�߂��������s��
	if (FLT_EPSILON < fabs(diffX) && fabs(min - fabs(diffX)) < FLT_EPSILON)
	{
		col2.Object().Pos().X() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().x;
	}
	else if (FLT_EPSILON < fabs(diffY) && fabs(min - fabs(diffY)) < FLT_EPSILON)
	{
		col2.Object().Pos().Y() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().y;
	}
	else if (FLT_EPSILON < fabs(diffZ) && fabs(min - fabs(diffZ)) < FLT_EPSILON)
	{
		col2.Object().Pos().Z() -= Dx12Wrapper::Instance().GetDeltaTime() * col2.Object().Speed().z;
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

	//�����x�N�g���ɑ΂���䗦���擾���A��Βl��1���傫����΁i=�͂ݏo�Ă�����j�����ɉ��Z
	float s = XMVector3Dot(vecBetcolAndPoint, col._rightDir).m128_f32[0] / col.HalfLength().x;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().x * col._rightDir;
	}

	s = XMVector3Dot(vecBetcolAndPoint, col._upDir).m128_f32[0] / col.HalfLength().y;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().y * col._upDir;
	}

	s = XMVector3Dot(vecBetcolAndPoint, col._frontDir).m128_f32[0] / col.HalfLength().z;
	if (fabs(s) > 1)
	{
		vec += (1 - fabs(s)) * col.HalfLength().z * col._frontDir;
	}

	//������0�ȉ���������OBB�̒��ɍ��W�����荞��ł���
	if (XMVector3Length(vec).m128_f32[0] <= 0)
	{
		return true;
	}

	return false;
}

/// <summary>
/// OBB�ɒ����������Ă��邩�m�F����֐�
/// </summary>
/// <param name="col">OBB</param>
/// <param name="startPos">�����̎n�_</param>
/// <param name="direction">�����̌���</param>
/// <returns></returns>
bool
CollisionDetector::CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& direction)
{
	return true;
}

/// <summary>
/// �������ɑ΂����e���ꂽ�e�x�N�g���̑傫���̘a�����߂�֐�
/// </summary>
/// <param name="sep">������</param>
/// <param name="right">�E�x�N�g��</param>
/// <param name="up">��x�N�g��</param>
/// <param name="front">���ʃx�N�g��</param>
/// <returns>�x�N�g���̑傫���̘a</returns>
float
CollisionDetector::LenOnSeparateAxis(const Vector3& sep, const Vector3& right, const Vector3& up, const Vector3& front)
{
	//�������Ɗe�x�N�g���̓��ς����A���̐�Βl�����e���ꂽ�傫���ƂȂ�
	float r1 = fabs(XMVector3Dot(sep, right).m128_f32[0]);
	float r2 = fabs(XMVector3Dot(sep, up).m128_f32[0]);
	float r3 = fabs(XMVector3Dot(sep, front).m128_f32[0]);

	return r1 + r2 + r3;
}