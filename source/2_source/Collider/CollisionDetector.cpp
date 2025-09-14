#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/FbxActor.h"
#include "FBX/IFbx.h"
#include "FBX/FbxObject.h"
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
/// <param name="dir">���e����x�N�g��</param>
/// <param name="pos">���W</param>
/// <returns>����</returns>
float 
CollisionDetector::GetLengthBetweenColAndPos(shared_ptr<ICollider> col, const Vector3& dir, const Vector3& pos)
{
	if(col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		//���W��OBB�̒��S�̍���
		Vector3 diff = pos - *col->Center();
		//�����̃x�N�g���ւ̓��e
		float lenOnDir = abs(XMVector3Dot(diff, dir).m128_f32[0]);

		//OBB�̕����x�N�g���̃x�N�g���ւ̓��e�̘a���擾
		float sumOfVecOnDir = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			sumOfVecOnDir += tempBox->HalfLength()[i] * abs(XMVector3Dot(
				dir, tempBox->DirectionVectors()[i]).m128_f32[0]);
		}

		float lenBetColAndPos = lenOnDir - sumOfVecOnDir;

		return lenBetColAndPos;
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
		dynamic_cast<FbxActor*>(actor)->AdjustPos(diffBetColAndVec);

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
	return CheckOBBIntersection(col1, col2);
}

/// <summary>
/// OBB�ɐ����������Ă��邩�m�F����֐�
/// </summary>
/// <param name="col">OBB</param>
/// <param name="startPos">�����̎n�_</param>
/// <param name="endPos">�����̏I�_</param>
/// <returns>�����Ă��邩�ǂ���</returns>
Vector3
CollisionDetector::CheckColAndVec(shared_ptr<ICollider> col, const Vector3& startPos, const Vector3& endPos)
{
	//�����̌���
	Vector3 lineDir = endPos - startPos;
	Vector3 center = *col->Center();
	//OBB�Ɛ����̒��S�̍���
	Vector3 centerDiff = endPos - center;
	Vector3 a = startPos - center;
	//OBB�Ɛ������������W
	Vector3 colPoint = XMVectorZero();

	float dirLength = XMVector3Length(endPos - startPos).m128_f32[0];

	if (col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		float r, r0, r1;
		//OBB�̕����x�N�g���ɑ΂����S�����𓊉e���A�������Ɛ����̓��e�̘a���r���ďՓ˔���
		for (int i = 0; i < 3; i++)
		{
			//���S�����̓��e
			r = XMVector3Dot(centerDiff, tempBox->DirectionVectors()[i]).m128_f32[0];
			//�����x�N�g���̔�����
			r0 = tempBox->HalfLength()[i];
			//�����̕����x�N�g���ւ̓��e
			r1 = abs(XMVector3Dot(lineDir, tempBox->DirectionVectors()[i]).m128_f32[0]);
			if (abs(r) > r0 + r1)
			{
				return XMVectorZero();
			}
			else
			{
				colPoint[i] = r - r1;
				if (i == 1)
				{
					colPoint[i] = r0;
				}
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
/// OBB�ɍ��W�����荞��ł��邩�m�F����֐�
/// </summary>
/// <param name="col">�Ώۂ�OBB</param>
/// <param name="point">���W</param>
/// <returns>���荞��ł��邩�ǂ���</returns>
bool
CollisionDetector::CheckColAndPoint(shared_ptr<ICollider> col, const Vector3& point)
{
	//���W��OBB�̒��S�����ԃx�N�g�����擾
	auto vecBetcolAndPoint = *col->Center() - point;

	//�����x�N�g������͂ݏo�Ă���x�N�g��
	Vector3 vec = XMVectorSet(0, 0, 0, 0);

	if (col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		for (int i = 0; i < 3; i++)
		{
			//�����x�N�g���ɑ΂���䗦���擾���A��Βl��1���傫����΋����ɉ��Z
			float s = XMVector3Dot(
				vecBetcolAndPoint, 
				tempBox->DirectionVectors()[i]).m128_f32[0] / tempBox->HalfLength()[i];
			if (fabs(s) > 1)
			{
				vec += (1 - fabs(s)) * tempBox->HalfLength()[i] * 
					tempBox->DirectionVectors()[i];
			}
		}

		//������0�ȉ���������OBB�̒��ɍ��W�����荞��ł���
		if (XMVector3Length(vec).m128_f32[0] <= 0)
		{
			return true;
		}

		return false;
	}

	return false;
}

/// <summary>
/// OBB���m���Փ˂��Ă��邩���ʂ���֐�
/// </summary>
/// <param name="col1">OBB����1</param>
/// <param name="col2">OBB����2</param>
/// <returns>�Փ˂��Ă��邩</returns>
bool 
CollisionDetector::CheckOBBIntersection(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2)
{
	//�eOBB�̉E�A��A���ʃx�N�g��
	Vector3 center1 = *col1->Center();
	Vector3 center2 = *col2->Center();

	//�o����OBB�̒��S�����ԃx�N�g��
	Vector3 vecBetCenter = center2 - center1;

	//�����蔻�肪����̎�
	if (col1 == dynamic_pointer_cast<BoxCollider>(col1))
	{
		if (col2 == dynamic_pointer_cast<BoxCollider>(col2))
		{
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
		else
		{
			return true;
		}
	}
	//����̎�
	else if (col1 == dynamic_pointer_cast<SphereCollider>(col1))
	{
		if (col2 == dynamic_pointer_cast<BoxCollider>(col2))
		{
			auto tempSphere = dynamic_pointer_cast<SphereCollider>(col1);
			auto tempBox = dynamic_pointer_cast<BoxCollider>(col2);
			auto distance = *col2->Center() - *tempSphere->Center();

			float disInBox = 0.0f;
			Vector3 normalizedDis = XMVector3Normalize(distance);
			for (int i = 0; i < 3; i++)
			{
				disInBox += LenOnSeparateAxis(normalizedDis,
					tempBox->DirectionVectors()[i] * tempBox->HalfLength()[i]);
			}

			if (fabs(XMVector3Length(distance).m128_f32[0]) <= 
				fabs(tempSphere->Radius()) + fabs(disInBox))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return true;
		}		
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