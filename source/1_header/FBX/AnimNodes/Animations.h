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
