#pragma once
#include "Fbx/AnimNodes/AnimNode.h"

class FbxActor;
class FallNode : public AnimNode {
    // 外部コンポーネントへの参照（生ポインタか弱参照）
    class FbxActor* _owner;

public:
    // コンストラクタ
    FallNode(FbxActor* owner);

    // アニメーション開始時に呼び出す
    void OnStart() override;

    // アニメーションちゅうに呼び出す
    void OnUpdate(float& animTime) override;

    // アニメーション終了時に呼び出す
    void OnEnd() override;

private:
    // ジャンプアニメーション終了後に移行するアニメーション
    AnimEnum _nextAnim = WAIT00;
};