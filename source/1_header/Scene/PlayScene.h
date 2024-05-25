#pragma once
#include "Application.h"

#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

#include <chrono>

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class PlayScene
{
public:
	PlayScene(							//コンストラクタ
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~PlayScene();						//デストラクタ

	void Update();						//更新処理

	void SceneStart();					//シーン開始時の処理
	void SceneEnd();					//シーン終了時の処理

private:
	Application& _app;												//Applicationインスタンス
	Dx12Wrapper& _dx12;												//Dx12Wrapperインスタンス
	InputManager& _input;											//InputManagerインスタンス
	SoundManager& _sound;											//SoundManagerインスタンス

	vector<shared_ptr<FBXBase>> _actorAndObjs;						//FBXObjectとFBXActorをまとめたベクトル
	shared_ptr<FBXActor> _actor;									//アクターインスタンス

	shared_ptr<Renderer> _renderer;									//Rendererインスタンス
	shared_ptr<PeraRenderer> _pera;									//PeraRendererインスタンス

	XMVECTOR _direction;											//プレイヤー、カメラの進行ベクトル

	bool _isInLoad;													//ロード中であることを示す真理値

	LARGE_INTEGER _currentTime;										//ゲームの現在時間を格納する値
	LARGE_INTEGER _updatedTime;										//ゲームの直前の時間を格納する値
	LARGE_INTEGER _beforeTime;										//ゲームの時間を一時保存しておくための値

	double _fps;													//1秒当たりに画面が切り替わる回数

	int _interval;													//フリップ間隔

	void PeraDraw();												//ペラポリゴンの描画処理をまとめた関数

	void GameDraw();												//ゲーム画面ポリゴンの描画処理をまとめた関数

	//二つの当たり判定が衝突しているか確認する関数
	bool CheckIntersect(map<string, float> col0, map<string, float> col1);		

	void DrawUpdate();												//描画関数

	void InputUpdate();												//入力更新関数

	void ModelDraw();												//モデル描画処理
	void EffectAndUIDraw();											//画面エフェクト描画処理

	void UpdateFPS();												//FPSを更新する関数
};