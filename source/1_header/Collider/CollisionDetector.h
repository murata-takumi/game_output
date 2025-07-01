#pragma once
#include "Application.h"

/// <summary>
/// �����蔻��̌v�Z���Ǘ�����N���X
/// </summary>
class BoxCollider;
class Dx12Wrapper;
class FBXActor;
class IFBX;
class FBXObject;
class Vector3;
class CollisionDetector
{
public:
	//�C���X�^���X��Ԃ�
	static CollisionDetector& Instance();

	//OBB�ƍ��W�̊Ԃ̋������擾����֐�
	float GetLengthBetweenColAndPos(
		const BoxCollider& col, 
		const Vector3& dir, 
		const Vector3& pos
	);

	//�A���I�ȏՓ˔��菈��
	bool CheckContinuousCollisionDetection(
		const BoxCollider& col, 
		const Vector3& dir, 
		const Vector3& currentPos,
		const float speed
	);

	//OBB���m�̓����蔻����m�F����֐�
	bool CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2);

	//OBB�Ƀx�N�g���������Ă��邩�m�F����֐�
	bool CheckColAndVec(
		const BoxCollider& col, 
		const Vector3& startPos, 
		const Vector3& endPos
	);

	//OBB�ɍ��W�������Ă��邩�m�F����֐�
	bool CheckColAndPoint(const BoxCollider& col, const Vector3& point);

private:
	//���[�v���񂵂�OBB���m���ڐG���Ă��邩���f����
	bool CheckOBBIntersection(const BoxCollider& col1, const BoxCollider& col2);

	//�������ɓ��e���ꂽ�����̒������擾����
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec);
};