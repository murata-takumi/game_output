#pragma once
#include "FBX/AnimNodes/JUMP00Node.h"
#include "FBX/FBXActor.h"

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
/// 毎フレーム更新する関数
/// </summary>
/// <param name="animTime">現在の経過時間</param>
void
JUMP00Node::Update(float animTime)
{
	AnimNode::Update(animTime);

	//一定時間経過したらアニメーション遷移できるようにする
	if (animTime >= 35.0f)
	{
		_actor->SetCanChangeAnim(true);
		_actor->SetOnGround(true);
	}
}

/// <summary>
/// アニメーション開始時に呼び出す関数
/// </summary>
void
JUMP00Node::StartAnimation()
{
	AnimNode::StartAnimation();

	//アニメーション速度を設定
	_actor->SetAnimationTime(8.5f);
	//別アニメーションに遷移しないようにする
	_actor->SetCanChangeAnim(false);
	//空中にいると設定
	_actor->SetOnGround(false);
	//ループしないようにする
	_actor->SetIsInLoop(false);
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