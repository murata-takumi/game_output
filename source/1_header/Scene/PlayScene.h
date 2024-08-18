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
	//コンストラクタ
	PlayScene();
	//デストラクタ
	~PlayScene();						

	//更新処理
	void Update();						

	//シーン開始時の処理
	void SceneStart();				
	//シーン終了時の処理
	void SceneEnd();					

private:
	//FBXObjectとFBXActorをまとめたベクトル
	vector<shared_ptr<FBXBase>> _actorAndObjs;						
	//アクターインスタンス
	shared_ptr<FBXActor> _actor;
	//床インスタンス
	shared_ptr<FBXObject> _ground;									

	//並列処理したいスレッドのベクトル
	vector<thread> _ths;

	//プレイヤー、カメラの進行ベクトル
	XMVECTOR _direction;

	//ロード中であることを示す真理値
	bool _isInLoad;													

	//ゲームの現在時間を格納する値
	LARGE_INTEGER _currentTime;							
	//ゲームの直前の時間を格納する値
	LARGE_INTEGER _updatedTime;						
	//ゲームの時間を一時保存しておくための値
	LARGE_INTEGER _beforeTime;										

	//1秒当たりに画面が切り替わる回数
	double _fps;													

	//フリップ間隔
	int _interval;													

	//オブジェクトを生成する関数
	template<class className> void InstantiateObject(const wchar_t* path);

	//ペラポリゴンの描画処理をまとめた関数
	void PeraDraw();	
	//ゲーム画面ポリゴンの描画処理をまとめた関数
	void GameDraw();												

	//二つの当たり判定が衝突しているか確認する関数
	bool CheckIntersect(map<string, float> col0, map<string, float> col1);

	//描画関数
	void DrawUpdate();		
	//入力更新関数
	void InputUpdate();
	//FPSを更新する関数
	void FPSUpdate();

	//モデル描画処理
	void ModelDraw();												
	//画面エフェクト描画処理
	void EffectAndUIDraw();											
};