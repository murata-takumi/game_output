#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// RUN00_F�����s�����Ƃ��ɌĂяo�����N���X
/// </summary>
class FBXActor;
class RUN00_FNode :public AnimNode
{
public:
	//�R���X�g���N�^
	RUN00_FNode(FBXActor* actor, AnimEnum anim);
};