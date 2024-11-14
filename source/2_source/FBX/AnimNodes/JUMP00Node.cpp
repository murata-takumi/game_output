#pragma once
#include "FBX/AnimNodes/JUMP00Node.h"
#include "FBX/FBXActor.h"

//アニメーションの開始時間
const float START_TIME = 8.5f;
//別アニメーションへの遷移が出来るようになる時間
const float TRANSITION_TIME = 35.0f;

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
JUMP00Node::JUMP00Node(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}

/// <summary>
/// アニメーション開始時に呼び出す関数
/// </summary>
void
JUMP00Node::StartAnimation()
{
	AnimNode::StartAnimation();

	//開始時間を少し後に設定
	_actor->SetAnimationTime(START_TIME);
	//別アニメーションに遷移しないようにする
	_actor->SetCanChangeAnim(false);
	//ループしないようにする
	_actor->SetIsInLoop(false);
}

/// <summary>
/// 毎フレーム更新する関数
/// </summary>
/// <param name="animTime">現在の経過時間</param>
void
JUMP00Node::Update(float& animTime)
{
	AnimNode::Update(animTime);

	//一定時間経過したらアニメーション遷移できるようにする
	if (animTime >= TRANSITION_TIME)
	{
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// アニメーション終了時に呼び出す関数
/// </summary>
void
JUMP00Node::EndAnimation()
{
	//ループできるようにする
	_actor->SetIsInLoop(true);
}