#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Scene/SceneComposition.h"
#include "Scene/TitleScene.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
TitleScene::TitleScene():IScene()
{
	//シーンの共通機能を初期化
	_sceneComp = make_shared<SceneComposition>();
	//関数も上書き
	_sceneComp->_effectAndUiDraw = std::bind(&TitleScene::EffectAndUIDraw, this);
	_sceneComp->_modelDraw = std::bind(&TitleScene::ModelDraw, this);
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

}

/// <summary>
/// 更新処理
/// </summary>
void
TitleScene::Update()
{
	//入力を更新
	_sceneComp->InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && _sceneComp->_canInput)
	{
		//開始ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//ゲームシーンへ遷移
			_sceneComp->ChangeScene(SceneNames::Play);

			return;
		}

		//終了ボタンの上で左クリック
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//効果音
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//操作不可にする
			_sceneComp->_canInput = false;

			auto exitFunc = ([&]()
				{
					//フェードアウト
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//ゲームを終了
					Application::Instance().ExitApp();

					return;
				}
			);
			_sceneComp->ParallelProcess(exitFunc);

			return;
		}
	}

	//描画処理
	_sceneComp->DrawUpdate();
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