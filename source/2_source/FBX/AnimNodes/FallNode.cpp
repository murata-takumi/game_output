#pragma once
#include "FBX/AnimNodes/FallNode.h"
#include "FBX/FBXActor.h"

//ループの開始、終了時間
const float LOOP_BEGIN_TIME = 30.0f;
const float LOOP_END_TIME = 31.0f;
//アニメーション速度
const float ANIM_SPEED = 0.1f;

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
	_actor->SetAnimationSpeed(ANIM_SPEED);
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
	if (animTime <= LOOP_BEGIN_TIME)
	{
		animTime = LOOP_BEGIN_TIME;
		_actor->SetAnimationSpeed(ANIM_SPEED);
	}
	else if (LOOP_END_TIME < animTime)
	{
		animTime = LOOP_END_TIME;
		_actor->SetAnimationSpeed(-ANIM_SPEED);
	}

	//地面に着いたら別アニメーションに遷移できるようにする
	if (_actor->GetOnGround())
	{
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// アニメーション終了時に呼び出す関数
/// </summary>
void
FallNode::EndAnimation()
{
	//アニメーション速度を元に戻す
	_actor->SetAnimationSpeed(1.0f);
}