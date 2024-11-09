#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// FALL�����s�����Ƃ��ɌĂяo�����N���X
/// </summary>
class FBXActor;
class FallNode :public AnimNode
{
public:

	FallNode(FBXActor* actor, AnimEnum anim);

	void StartAnimation()override;

	void Update(float& animTime)override;

	void EndAnimation()override;
};
