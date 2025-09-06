#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Scene/SceneComposition.h"
#include "Scene/TitleScene.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// シーン開始時の処理
/// </summary>
void
TitleScene::SceneStart()
{
	//関数も上書き
	SceneComposition::Instance().SetEffectAndUiDraw(std::bind(
		&TitleScene::EffectAndUIDraw, this));
	SceneComposition::Instance().SetModelDraw(std::bind(&TitleScene::ModelDraw, this));

	//ロード完了
	SceneComposition::Instance().SetCanInput(true);

	//フェードイン処理を並列処理
	auto startFunc = [&]()
	{
		Dx12Wrapper::Instance().Fade(1.0f, 0.0f);
	};
	SceneComposition::Instance().ParallelProcess(startFunc);
}

/// <summary>
/// 更新処理
/// </summary>
void
TitleScene::Update()
{
	//入力を更新
	SceneComposition::Instance().InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && SceneComposition::Instance().GetCanInput())
	{
		//開始ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//ゲームシーンへ遷移
			SceneComposition::Instance().ChangeScene(SceneNames::Play);

			return;
		}

		//終了ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//操作不可にする
			SceneComposition::Instance().SetCanInput(false);

			auto exitFunc = ([&]()
				{
					//フェードアウト
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//ゲームを終了
					Application::Instance().ExitApp();

					return;
				}
			);
			SceneComposition::Instance().ParallelProcess(exitFunc);

			return;
		}
	}

	//描画処理
	SceneComposition::Instance().DrawUpdate();
}

/// <summary>
/// シーン終了時の処理
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// UI描画処理
/// </summary>
void
TitleScene::ModelDraw()
{
	SpriteManager::Instance().ButtonDraw();
}

/// <summary>
/// エフェクト上の描画を行う関数
/// </summary>
void
TitleScene::EffectAndUIDraw()
{

}