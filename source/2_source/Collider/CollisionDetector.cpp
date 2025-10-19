#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/FbxActor.h"
#include "FBX/IFbx.h"
#include "FBX/FbxObject.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

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
/// OBB�ƍ��W�̊Ԃ̋�����Ԃ��֐�
/// </summary>
/// <param name="col">OBB</param>
/// <param name="pos">���W</param>
/// <returns>����</returns>
float 
CollisionDetector::GetLengthBetweenColAndPos(shared_ptr<ICollider> col, const Vector3& pos)
{
	//���W��OBB�̒��S�̍���
	Vector3 diff = pos - *col->Center();

	//�����̋���
	float lenOnDiff = XMVector3Length(diff).m128_f32[0];

	if(col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		//OBB�̕����x�N�g���̃x�N�g���ւ̓��e�̘a���擾
		float sumOfLenOnDir = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			float s = fabs(XMVector3Dot(diff, tempBox->DirectionVectors()[i]).m128_f32[0]) / tempBox->HalfLength()[i];

			if (s > 1)
			{
				sumOfLenOnDir += (1 - s) * tempBox->HalfLength()[i];
			}
		}

		//OBB���̋����ƍ����̍�
		float lenBetColAndPos = lenOnDiff - sumOfLenOnDir;

		return lenBetColAndPos;
	}
	else
	{
		auto tempSphere = dynamic_pointer_cast<SphereCollider>(col);

		return lenOnDiff - tempSphere->Radius();
	}
}

/// <summary>
/// �A���I�ȏՓ˔��菈��
/// </summary>
/// <param name="actor">���W���Y�����Ώۂ̃A�N�^�[</param>
/// <param name="col">�ՓˑΏۂ�OBB</param>
/// <param name="dir">�ړ�����</param>
/// <param name="pos">���݂̍��W</param>
/// <param name="speed">�ړ����x</param>
/// <returns>�Փ˂��邩�ǂ���</returns>
bool 
CollisionDetector::CheckContinuousCollisionDetection(
	IFbx* actor,
	shared_ptr<ICollider> col, 
	const Vector3& dir, 
	const Vector3& currentPos, 
	const float speed)
{
	//1�t���[����̍��W���擾
	Vector3 nextPos = currentPos + (dir * speed * Dx12Wrapper::Instance().GetDeltaTime());
	//OBB�ƍ��W�̍�
	Vector3 diffBetColAndVec = CheckColAndVec(col, currentPos, nextPos);

	//���݂�1�t���[����̍��W�̃x�N�g����OBB�ƌ����i=1�t���[����ɏՓ˂���j�Ȃ�^
	if (XMVector3Length(diffBetColAndVec).m128_f32[0] > 0.0f)
	{
		return true;
	}

	return false;
}

/// <summary>
/// �������藝���OBB���Փ˂��Ă��邩�m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩�ǂ���</returns>
bool
CollisionDetector::CheckColAndCol(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2)
{
	Vector3 vecBetCenter = *col2->Center() - *col1->Center();

	return CheckBoxAndBox(col1, col2, vecBetCenter) || 
		CheckBoxAndSphere(col2, col1, vecBetCenter) || 
		CheckSphereAndSphere(col1, col2, vecBetCenter);
}

/// <summary>
/// �����蔻��ɐ����������Ă��邩�m�F����֐�
/// </summary>
/// <param name="col">�����蔻��</param>
/// <param name="startPos">�����̎n�_</param>
/// <param name="endPos">�����̏I�_</param>
/// <returns>�����Ă��邩�ǂ���</returns>
Vector3
CollisionDetector::CheckColAndVec(shared_ptr<ICollider> col, const Vector3& startPos, const Vector3& endPos)
{
	//����
	Vector3 line = endPos - startPos;
	//OBB�Ɛ����̒��S�̍���
	Vector3 diffBetCenter = endPos - *col->Center();
	//OBB�Ɛ������������W
	Vector3 colPoint = XMVectorZero();

	if (col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		float lenOnDiffBetCenter, halfLen, lineLenOnDir;
		//OBB�̕����x�N�g���ɑ΂����S�����𓊉e���A�������Ɛ����̓��e�̘a���r���ďՓ˔���
		for (int i = 0; i < 3; i++)
		{
			//���S�����̓��e
			lenOnDiffBetCenter = XMVector3Dot(diffBetCenter, tempBox->DirectionVectors()[i]).m128_f32[0];
			//�����x�N�g���̔�����
			halfLen = tempBox->HalfLength()[i];
			//�����̕����x�N�g���ւ̓��e
			lineLenOnDir = abs(XMVector3Dot(line, tempBox->DirectionVectors()[i]).m128_f32[0]);

			if (abs(lenOnDiffBetCenter) > halfLen + lineLenOnDir)
			{
				return XMVectorZero();
			}
			else
			{
				colPoint[i] = lenOnDiffBetCenter - lineLenOnDir;
				//if (i == 1)
				//{
				//	colPoint[i] = halfLen;
				//}
			}

		}
	}
	else if(col == dynamic_pointer_cast<SphereCollider>(col))
	{
		return XMVectorZero();
	}

	return colPoint + *col->Center();
}

/// <summary>
/// ��`���m�̓����蔻����m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩</returns>
bool
CollisionDetector::CheckBoxAndBox(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<BoxCollider>(col1) || !dynamic_pointer_cast<BoxCollider>(col2))
	{
		return false;
	}

	auto tempBox1 = dynamic_pointer_cast<BoxCollider>(col1);
	auto tempBox2 = dynamic_pointer_cast<BoxCollider>(col2);

	for (int i = 0; i < 3; i++)
	{
		//�e�����x�N�g���̕������ւ̓��e�̘a
		float r = 0;
		for (int j = 0; j < 3; j++)
		{
			r += LenOnSeparateAxis(tempBox1->DirectionVectors()[i],
				tempBox2->DirectionVectors()[j] * tempBox2->HalfLength()[j]);
		}

		//���S�ԃx�N�g���̕������ւ̓��e
		float s = XMVector3Dot(vecBetCenter, tempBox1->DirectionVectors()[i]).m128_f32[0];

		//��r���āA���S�ԃx�N�g���̓��e�̕����傫����ΏՓ˂��Ă��Ȃ�
		if (fabs(s) > r + tempBox1->HalfLength()[i])
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// ���`���m�̓����蔻����m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩</returns>
bool
CollisionDetector::CheckSphereAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<SphereCollider>(col1) || !dynamic_pointer_cast<SphereCollider>(col2))
	{
		return false;
	}

	//���̔��a�̘a���擾���A���S�x�N�g���̋����Ɣ�r
	auto tempSphe1 = dynamic_pointer_cast<SphereCollider>(col1);
	auto tempSphe2 = dynamic_pointer_cast<SphereCollider>(col2);
	auto sumOfRadius = tempSphe1->Radius() + tempSphe2->Radius();

	if (pow(sumOfRadius,2) >= XMVector3LengthSq(vecBetCenter).m128_f32[0])
	{
		return true;
	}
	else
	{
		return false;
	}
}
/// <summary>
/// ��`�Ƌ��`�̓����蔻����m�F����֐�
/// </summary>
/// <param name="col1">�����蔻�肻��1</param>
/// <param name="col2">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩</returns>
bool
CollisionDetector::CheckBoxAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<BoxCollider>(col1) || !dynamic_pointer_cast<SphereCollider>(col2))
	{
		return CheckBoxAndSphere(col2, col1, vecBetCenter);
	}

	auto tempBox = dynamic_pointer_cast<BoxCollider>(col1);
	auto tempSphere = dynamic_pointer_cast<SphereCollider>(col2);

	auto dis = XMVector3Length(vecBetCenter).m128_f32[0];
	auto radius = tempSphere->Radius();

	float disInBox = GetLengthBetweenColAndPos(tempBox, *tempSphere->Center());

	if (fabs(disInBox) - fabs(dis) <= fabs(tempSphere->Radius()))
	{
		return true;
	}
	else
	{
		return false;
	}
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