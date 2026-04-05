#include "Vector3.h"
#include "Fbx/FBXActor.h"
#include "Fbx/AnimNodes/FallNode.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="owner">アニメーションを紐づけるアクター</param>
FallNode::FallNode(FbxActor* owner) : AnimNode(FALL), _owner(owner) {}

/// <summary>
/// アニメーション開始時に呼び出す
/// </summary>
void
FallNode::OnStart() {
	// 開始時間を少し後に設定
	_owner->SetAnimationTime(LOOP_BEGIN_TIME);

	// 別アニメーションに遷移しないようにする
	_owner->SetCanChangeAnim(false);
	_owner->SetAnimationSpeed(ANIM_SPEED);
}

/// <summary>
/// アニメーション中に呼び出す
/// </summary>
/// <param name="animTime">再生時間</param>
void
FallNode::OnUpdate(float& animTime) {
	// ループしているように見せる為の苦肉の策
	if (animTime <= LOOP_BEGIN_TIME)
	{
		animTime = LOOP_BEGIN_TIME;
		_owner->SetAnimationSpeed(ANIM_SPEED);
	}
	else if (LOOP_END_TIME < animTime)
	{
		animTime = LOOP_END_TIME;
		_owner->SetAnimationSpeed(-ANIM_SPEED);
	}

	// 着地したらアニメーション切り替え
	if (_owner->GetContinuousOnGround(XMVectorSet(0, -1, 0, 0), _owner->CurrentPosition(),
		45.0f * GRAVITY_ACCERALATION))
	{
		_owner->SetCanChangeAnim(true);
	}
}

/// <summary>
/// アニメーション終了時に呼び出す
/// </summary>
void
FallNode::OnEnd()
{		
	// アニメーション速度を元に戻す
	_owner->SetAnimationSpeed(1.0f);
}