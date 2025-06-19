#include "Scene/TitleScene.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
TitleScene::TitleScene():BaseScene()
{

}

/// <summary>
/// デストラクタ
/// </summary>
TitleScene::~TitleScene()
{

}

/// <summary>
/// シーン開始時の処理
/// </summary>
void
TitleScene::SceneStart()
{
	BaseScene::SceneStart();
}

/// <summary>
/// 更新処理
/// </summary>
void
TitleScene::Update()
{
	//入力を更新
	BaseScene::InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && BaseScene::_canInput)
	{
		//開始ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//ゲームシーンへ遷移
			BaseScene::ChangeScene(SceneNames::Play);

			return;
		}

		//終了ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//操作不可にする
			BaseScene::_canInput = false;

			auto exitFunc = ([&]()
				{
					//フェードアウト
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//ゲームを終了
					Application::Instance().ExitApp();

					return;
				}
			);
			BaseScene::ParallelProcess(exitFunc);

			return;
		}
	}

	//描画処理
	BaseScene::DrawUpdate();
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