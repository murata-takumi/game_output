#pragma once
#include "Application.h"

#include "FBX/BoxCollider.h"
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
	bool CheckColAndVec(const BoxCollider& col, const XMVECTOR& startPos, const XMVECTOR& direction);

	//OBB�ɍ��W�������Ă��邩�m�F����֐�
	bool CheckColAndPoint(const BoxCollider& col, const XMVECTOR& point);

	//�������ɓ��e���ꂽ�����̒������擾����
	float LenOnSeparateAxis(const XMVECTOR& sep, const XMVECTOR& right, const XMVECTOR& up, const XMVECTOR& front = XMVectorZero());
};