#include "Scene/TitleScene.h"

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
/// 更新処理
/// </summary>
void
TitleScene::Update()
{
	//入力を更新
	InputUpdate();											


	if (InputManager::Instance().MouseTracker().leftButton == 
		Mouse::ButtonStateTracker::PRESSED && _isPlaying)
	{
		//開始ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnStart())				
		{
			//効果音
			SoundManager::Instance().CallSound(BUTTON);							

			//ゲームシーンへ遷移
			ChangeScene(SceneNames::Play);					

			return;
		}

		//終了ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnEnd())					
		{
			//効果音
			SoundManager::Instance().CallSound(BUTTON);							

			//操作不可にする
			_isPlaying = false;								

			auto exitFunc = ([&]()
				{
					//フェードアウト
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);					

					//ゲームを終了
					Application::Instance().ExitApp();							

					return;
				}
			);
			ParallelProcess(exitFunc);

			return;
		}
	}

	//描画処理
	DrawUpdate();	
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
/// シーン終了時の処理
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// 背景を描画する関数
/// </summary>
void
TitleScene::BackGroundDraw()
{
	//背景を描画
	BaseScene::BackGroundDraw();	
}