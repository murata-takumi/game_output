#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// JUMP00�����s�����Ƃ��ɌĂяo�����N���X
/// </summary>
class FBXActor;
class JUMP00Node :public AnimNode
{
public:

	JUMP00Node(FBXActor* actor, AnimEnum anim);

	void Update(float animTime)override;

	void StartAnimation()override;

	void EndAnimation()override;
};
