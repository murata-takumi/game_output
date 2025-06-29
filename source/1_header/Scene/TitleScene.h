#pragma once
#include "Scene/IScene.h"

/// <summary>
/// タイトルシーンを管理するクラス
/// </summary>
class SceneComponent;
class TitleScene : public IScene
{
public:
	//コンストラクタ
	TitleScene() = default;
	//デストラクタ
	~TitleScene() = default;

	//更新処理
	void Update() override;			

	//シーン開始時の処理
	void SceneStart() override;		
	//シーン終了時の処理
	void SceneEnd() override;		

	//UI描画処理
	void ModelDraw() override;
	//エフェクト・UIを描画する関数
	void EffectAndUIDraw();
protected:
};