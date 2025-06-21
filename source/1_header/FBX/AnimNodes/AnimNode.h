#pragma once
#include "Includes.h"

#include "FBX/AnimNodes/Animations.h"

/// <summary>
/// アニメーション実行時の各処理をまとめた基盤クラス
/// </summary>
class AnimNode
{
private:

	//実行するアニメーションの列挙体
	AnimEnum _anim;

	//それぞれアニメーション開始時、更新中、終了時に実行するコールバック
	function<void(void)> _startFunc;
	function<void(float)> _updateFunc;
	function<void(void)> _endFunc;

public:

	//コンストラクタ
	AnimNode(AnimEnum anim, const function<void(void)>& startFunc = nullptr, const function<void(float)>& updateFunc = nullptr, const function<void(void)>& endFunc = nullptr);


	//アニメーション開始時に呼び出す関数
	void StartAnimation();

	//毎フレーム呼び出す
	void Update(float& animTime);

	//アニメーション終了時に呼び出す
	void EndAnimation();

	//列挙体を取得する関数
	AnimEnum GetAnimEnum()const;
};