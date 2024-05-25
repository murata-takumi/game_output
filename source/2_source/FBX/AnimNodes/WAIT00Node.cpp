#pragma once
#include "FBX/AnimNodes/WAIT00Node.h"
#include "FBX/FBXActor.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
WAIT00Node::WAIT00Node(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}