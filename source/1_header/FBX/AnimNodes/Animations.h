#pragma once
#include "Application.h"

/// <summary>
/// アクターのアニメーションを決定するための列挙体
/// </summary>
enum AnimEnum
{
	WAIT00,
	RUN00_F,
	JUMP00,
	FALL,
	Length,
};

/// <summary>
/// 列挙体とアクターのアニメーション名の連想配列
/// 列挙体を指定してアニメーション名を取得できるようにするためこのような形にしている
/// </summary>
static map<AnimEnum, string> animations
{
	{WAIT00,"WAIT00"},
	{RUN00_F,"RUN00_F"},
	{JUMP00,"JUMP00"},
	{FALL,"JUMP00B"},
};

// アニメーションの開始時間
const float START_TIME = 8.5f;

// 次のアニメーションを決定するタイミング
const float SELECT_NEXT_ANIM_TIME = 30.0f;

// 別アニメーションへの遷移が出来るようになる時間
const float TRANSITION_TIME = 34.5f;

// 重力加速度
const float GRAVITY_ACCERALATION = 9.8f;

// ループの開始、終了時間
const float LOOP_BEGIN_TIME = 30.0f;
const float LOOP_END_TIME = 31.0f;

// アニメーション速度
const float ANIM_SPEED = 0.1f;