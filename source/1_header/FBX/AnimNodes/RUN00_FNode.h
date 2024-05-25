#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// RUN00_Fを実行したときに呼び出されるクラス
/// </summary>
class FBXActor;
class RUN00_FNode :public AnimNode
{
public:
	//コンストラクタ
	RUN00_FNode(FBXActor* actor, AnimEnum anim);
};