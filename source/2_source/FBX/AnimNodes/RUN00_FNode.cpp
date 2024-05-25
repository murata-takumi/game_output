#pragma once
#include "FBX/AnimNodes/RUN00_FNode.h"
#include "FBX/FBXActor.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
RUN00_FNode::RUN00_FNode(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}