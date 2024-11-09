#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FBXActor.h"

//全アニメーション共通で実行する処理

/// <summary>
/// コンストラクタ
/// アクターのポインタ、アニメーションに対応する列挙体を取得する
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
AnimNode::AnimNode(FBXActor* actor, AnimEnum anim)
	:_actor(actor), _anim(anim)
{

}

/// <summary>
/// アニメーション開始時に呼び出す関数
/// アクターにアニメーションを実行させる
/// </summary>
void
AnimNode::StartAnimation()
{
	_actor->BlendAnimation(_anim);
};

/// <summary>
/// アニメーション実行時、毎フレーム呼び出す関数
/// </summary>
/// <param name="animTime">アニメーションの再生時間</param>
void 
AnimNode::Update(float& animTime)
{

};

/// <summary>
/// アニメーション終了時に呼び出す関数
/// </summary>
void 
AnimNode::EndAnimation()
{

};

/// <summary>
/// 列挙体を返す関数
/// </summary>
/// <returns>列挙体</returns>
AnimEnum
AnimNode::GetAnimEnum()const
{
	return _anim;
}