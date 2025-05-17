#pragma once
#include "Application.h"

#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Scene/BaseScene.h"

#include <chrono>

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class PlayScene : public BaseScene
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

protected:
	//FBXObjectとFBXActorをまとめたベクトル
	map<string, shared_ptr<FBXBase>> _actorAndObjs;
	//アクターインスタンス
	shared_ptr<FBXActor> _actor;
	//床インスタンス
	shared_ptr<FBXObject> _ground;									

	//並列処理したいスレッドのベクトル
	vector<thread> _ths;

	//プレイヤー、カメラの進行ベクトル
	XMVECTOR _direction;							

	char* _currentFootObjName;

	//オブジェクトを生成する関数
	template<class className> void InstantiateObject(const wchar_t* path, string name,XMFLOAT3 size, XMFLOAT3 pos = XMFLOAT3(0.0f,0.0f,0.0f));												

	//OBB同士の当たり判定を確認する関数
	bool CheckColliders(const BoxCollider& col1, const BoxCollider& col2);

	//OBBにベクトルが入っているか確認する関数
	bool CheckColAndPoint(const BoxCollider& col, const XMVECTOR& point);

	//分離軸に投影された線分の長さを取得する
	float LenOnSeparateAxis(const XMVECTOR& sep, const XMVECTOR& right, const XMVECTOR& up, const XMVECTOR& front = XMVectorZero());

	//モデル描画処理
	void ModelDraw() override;												
	//画面エフェクト描画処理
	void EffectAndUIDraw() override;											
};