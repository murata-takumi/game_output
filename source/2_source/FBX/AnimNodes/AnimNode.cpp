#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FBXActor.h"

//全アニメーション共通で実行する処理

/// <summary>
/// コンストラクタ
/// アクターのポインタ、アニメーションに対応する列挙体を取得する
/// </summary>
/// <param name="actor">ポインタ</param>
/// <param name="anim">列挙体</param>
AnimNode::AnimNode(AnimEnum anim, const function<void(void)>& startFunc, const function<void(float)>& updateFunc, const function<void(void)>& endFunc)
	:_anim(anim),_startFunc(startFunc),_updateFunc(updateFunc),_endFunc(endFunc)
{

}

/// <summary>
/// アニメーション開始時に呼び出す関数
/// アクターにアニメーションを実行させる
/// </summary>
void
AnimNode::StartAnimation()
{
	//コールバック実行
	if (_startFunc != nullptr)
	{
		_startFunc();
	}
};

/// <summary>
/// アニメーション実行時、毎フレーム呼び出す関数
/// </summary>
/// <param name="animTime">アニメーションの再生時間</param>
void 
AnimNode::Update(float& animTime)
{
	//コールバック実行
	if (_updateFunc != nullptr)
	{
		_updateFunc(animTime);
	}
};

/// <summary>
/// アニメーション終了時に呼び出す関数
/// </summary>
void 
AnimNode::EndAnimation()
{
	//コールバック実行
	if (_endFunc != nullptr)
	{
		_endFunc();
	}
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