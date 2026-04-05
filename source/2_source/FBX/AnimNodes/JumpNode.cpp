#include "Vector3.h"
#include "Fbx/FBXActor.h"
#include "Fbx/AnimNodes/JumpNode.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="owner">アニメーションを紐づけるアクター</param>
JumpNode::JumpNode(FbxActor* owner) : AnimNode(JUMP00), _owner(owner) {}

/// <summary>
/// アニメーション開始時に呼び出す
/// </summary>
void 
JumpNode::OnStart() {
    _owner->SetAnimationTime(START_TIME);
    _owner->SetCanChangeAnim(false);
    _owner->SetIsInLoop(false);
    _owner->SetRejectBone(false);
}

/// <summary>
/// アニメーション中に呼び出す
/// </summary>
/// <param name="animTime">再生時間</param>
void 
JumpNode::OnUpdate(float& animTime) {
    if (animTime > SELECT_NEXT_ANIM_TIME) {
        // 遷移先の判定ロジックをここに集約
        _nextAnim = DetermineNextAnim();
    }
    if (animTime > TRANSITION_TIME) {
        _owner->SetCanChangeAnim(true);
        _owner->SetRejectBone(true);
        _owner->SetAnimationNode(_nextAnim);
    }
}

/// <summary>
/// アニメーション終了時に呼び出す
/// </summary>
void
JumpNode::OnEnd()
{
	_owner->SetIsInLoop(true);
}

/// <summary>
/// 地面判定ロジック
/// </summary>
/// <returns>次のアニメーション</returns>
AnimEnum 
JumpNode::DetermineNextAnim()
{
	if (_owner->GetContinuousOnGround(XMVectorSet(0, -1, 0, 0), _owner->CurrentPosition(),
		45.0f * GRAVITY_ACCERALATION))
	{
		// 入力されているかどうかで遷移先のアニメーションを決める
		if (XMVector3Length(_owner->GetInputVec()).m128_f32[0] > 0.0f)
		{
			// 移動アニメーション
			return RUN00_F;
		}
		else
		{
			// 待機アニメーション
			return WAIT00;
		}
	}
	else
	{
		// 落下アニメーション
		return FALL;
	}
}