#pragma once
#include "Application.h"

/// <summary>
/// �����蔻��̌v�Z���Ǘ�����N���X
/// </summary>
class BoxCollider;
class Dx12Wrapper;
class FbxActor;
class ICollider;
class IFbx;
class FbxObject;
class Vector3;
class CollisionDetector
{
public:
	//�C���X�^���X��Ԃ�
	static CollisionDetector& Instance();

	//OBB�ƍ��W�̊Ԃ̋������擾����֐�
	float GetLengthBetweenColAndPos(
		shared_ptr<ICollider> col, 
		const Vector3& dir, 
		const Vector3& pos
	);

	//�A���I�ȏՓ˔��菈��
	bool CheckContinuousCollisionDetection(
		shared_ptr<ICollider> col, 
		const Vector3& dir, 
		const Vector3& currentPos,
		const float speed
	);

	//OBB���m�̓����蔻����m�F����֐�
	bool CheckColAndCol(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2);

	//OBB�Ƀx�N�g���������Ă��邩�m�F����֐�
	bool CheckColAndVec(
		shared_ptr<ICollider> col, 
		const Vector3& startPos, 
		const Vector3& endPos
	);

	//OBB�ɍ��W�������Ă��邩�m�F����֐�
	bool CheckColAndPoint(shared_ptr<ICollider> col, const Vector3& point);

private:
	//���[�v���񂵂�OBB���m���ڐG���Ă��邩���f����
	bool CheckOBBIntersection(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2);

	//�������ɓ��e���ꂽ�����̒������擾����
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec);
};