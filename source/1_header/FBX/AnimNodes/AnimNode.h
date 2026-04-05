#pragma once
#include "Includes.h"

#include <functional>

#include "FBX/AnimNodes/Animations.h"

/// <summary>
/// アニメーション処理の基盤クラス
/// </summary>
class AnimNode {
protected:
    // アニメーション
    AnimEnum _anim;

public:

    /// <summary>
    /// コンストラクタ
    /// アニメーションを紐づける
    /// </summary>
    /// <param name="anim">アニメーション</param>
    explicit AnimNode(AnimEnum anim) : _anim(anim) {}
    virtual ~AnimNode() = default;

    // デフォルトは何もしない
    virtual void OnStart() {}
    virtual void OnUpdate(float& animTime) {}
    virtual void OnEnd() {}

    // 紐付けられているアニメーションを返す
    AnimEnum GetAnimEnum() const { return _anim; }
};