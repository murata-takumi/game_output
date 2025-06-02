#pragma once
#include "Application.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �����蔻��̌v�Z���Ǘ�����N���X
/// </summary>
class CollisionDetector
{
public:
	static CollisionDetector& Instance();

	//OBB���m�̓����蔻����m�F����֐�
	bool CheckColAndCol(const BoxCollider& col1, const BoxCollider& col2);

	//OBB�Ƀx�N�g���������Ă��邩�m�F����֐�
	bool CheckColAndVec(const BoxCollider& col, const Vector3& startPos, const Vector3& endPos);

	//OBB�ɍ��W�������Ă��邩�m�F����֐�
	bool CheckColAndPoint(const BoxCollider& col, const Vector3& point);

private:
	//���[�v���񂵂�OBB���m���ڐG���Ă��邩���f����
	bool CheckOBBIntersection(const BoxCollider& col1, const BoxCollider& col2);

	//�������ɓ��e���ꂽ�����̒������擾����
	float LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec);
};