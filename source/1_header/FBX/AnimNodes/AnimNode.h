#pragma once
#include "FBX/AnimNodes/Animations.h"
#include "Application.h"

/// <summary>
/// アニメーション実行時の各処理をまとめた基盤クラス
/// </summary>
class FBXActor;
class AnimNode
{
protected:
	//アクターのポインタ
	FBXActor* _actor;

	//実行するアニメーションの列挙体
	AnimEnum _anim;

public:

	//コンストラクタ
	AnimNode(FBXActor* actor, AnimEnum anim);

	//毎フレーム呼び出す
	virtual void Update(float animTime);

	//アニメーション開始時に呼び出す関数
	virtual void StartAnimation();

	//アニメーション終了時に呼び出す
	virtual void EndAnimation();

	//列挙体を取得する関数
	AnimEnum GetAnimEnum()const;
};