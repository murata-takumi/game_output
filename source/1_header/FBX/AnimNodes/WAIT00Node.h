#pragma once
#include "FBX/AnimNodes/AnimNode.h"

/// <summary>
/// WAIT_00を実行した時に呼び出されるクラス
/// </summary>
class WAIT00Node :public AnimNode
{
public:
	//コンストラクタ
	WAIT00Node(FBXActor* actor, AnimEnum anim);
};
