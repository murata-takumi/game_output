#pragma once
#include "Application.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

#include <chrono>

/// <summary>
/// 各シーンオブジェクトの基幹クラス
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class BaseScene
{
	friend ImGuiManager;

public:

	//コンストラクタ
	BaseScene();
	//デストラクタ
	virtual ~BaseScene();

	//更新処理
	virtual void Update() = 0;				
	//シーン開始時の処理
	virtual void SceneStart();		
	//シーン終了時の処理
	virtual void SceneEnd();																	
protected:
	//ロード中であることを示す真理値
	static bool _isLoading;									
	//操作可能であることを示す真理値
	static bool _isPlaying;									

	//ゲームの現在時間を格納する値
	static LARGE_INTEGER _currentTime;						
	//ゲームの直前の時間を格納する値
	static LARGE_INTEGER _updatedTime;						
	//ゲームの時間を一時保存しておくための値
	static LARGE_INTEGER _beforeTime;						

	//1秒当たりに画面が切り替わる回数
	double _fps;											

	//フリップ間隔
	int _interval;											

	//シーン遷移する為の関数
	void ChangeScene(SceneNames name);						

	//背景を描画する関数
	virtual void BackGroundDraw();							

	virtual void ModelDraw();

	//エフェクト・UIを描画する関数
	virtual void EffectAndUIDraw();							

	//ペラポリゴンの描画処理をまとめた関数
	void PeraDraw();										

	//ゲーム画面ポリゴンの描画処理をまとめた関数
	void GameDraw();										

	//描画関数
	void DrawUpdate();										
	//入力更新関数
	void InputUpdate();			
	//FPSを更新する関数
	void UpdateFPS();										

	//ラムダ式を受け取り並列処理を行う関数
	void ParallelProcess(function<void(void)> func);		
};