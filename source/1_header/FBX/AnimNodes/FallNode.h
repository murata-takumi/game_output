#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// FALLを実行したときに呼び出されるクラス
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
