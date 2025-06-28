#pragma once
#include "Application.h"

/// <summary>
/// 各シーンオブジェクトに持たせるコンポジションクラス
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class SceneComposition
{
public:
	static SceneComposition& Instance();

	//モデルの描画を行う関数ラッパー
	std::function<void()> _modelDraw;
	//エフェクト、UIの描画を行う関数ラッパー
	std::function<void()> _effectAndUiDraw;

	//並列処理を行う時、リソースを排他制御する為の変数
	mutex _mtx;

	//ゲームの現在時間を格納する値
	LARGE_INTEGER _currentTime;
	//ゲームの直前の時間を格納する値
	LARGE_INTEGER _updatedTime;
	//ゲームの時間を一時保存しておくための値
	LARGE_INTEGER _beforeTime;

	//1秒当たりに画面が切り替わる回数
	double _fps;

	//ロード中であることを示す真理値
	bool _canInput;

	//シーン遷移する為の関数
	void ChangeScene(SceneNames name);

	//ペラポリゴンの描画処理をまとめた関数
	void PeraDraw();
	//ゲーム画面ポリゴンの描画処理をまとめた関数
	void GameDraw();

	//描画関数
	void DrawUpdate();
	//入力更新関数
	void InputUpdate();
	//フレームレートを更新する関数
	void FPSUpdate();

	//ラムダ式を受け取り並列処理を行う関数
	void ParallelProcess(function<void(void)> func);
};