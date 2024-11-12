#pragma once
#include "Scene/BaseScene.h"

/// <summary>
/// タイトルシーンを管理するクラス
/// </summary>
class TitleScene : public BaseScene
{
public:
	//コンストラクタ
	TitleScene();
	//デストラクタ
	~TitleScene();					

	//更新処理
	void Update() override;			

	//シーン開始時の処理
	void SceneStart() override;		
	//シーン終了時の処理
	void SceneEnd() override;		

protected:
};