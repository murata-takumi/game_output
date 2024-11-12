#pragma once
#include "FBX/AnimNodes/FallNode.h"
#include "FBX/FBXActor.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
FallNode::FallNode(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}

/// <summary>
/// アニメーション開始時に呼び出す関数
/// </summary>
void
FallNode::StartAnimation()
{
	AnimNode::StartAnimation();

	//別アニメーションに遷移しないようにする
	_actor->SetCanChangeAnim(false);
	_actor->SetAnimationSpeed(0.1f);
}

/// <summary>
/// 毎フレーム更新する関数
/// </summary>
/// <param name="animTime">現在の経過時間</param>
void
FallNode::Update(float& animTime)
{
	AnimNode::Update(animTime);

	//ループしているように見せる為の苦肉の策
	if (animTime <= 30.0f)
	{
		animTime = 30.0f;
		_actor->SetAnimationSpeed(0.1f);
	}
	else if (31.0f < animTime)
	{
		animTime = 31.0f;
		_actor->SetAnimationSpeed(-0.1f);
	}

	if (_actor->GetOnGround())
	{
		//別アニメーションに遷移できるようにする
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// アニメーション終了時に呼び出す関数
/// </summary>
void
FallNode::EndAnimation()
{
	_actor->SetAnimationSpeed(1.0f);
}