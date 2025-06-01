#pragma once
#include "Application.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "FBX/AssimpLoader.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "OcTree/OcTreeNode.h"
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

	OcTreeNode* _node;

	//プレイヤー、カメラの進行ベクトル
	Vector3 _direction;

	//オブジェクトを生成する関数
	template<class className> void InstantiateObject(const wchar_t* path, string name,Vector3 size, Vector3 pos = Vector3(0.0f,0.0f,0.0f));

	//モデル描画処理
	void ModelDraw() override;												
	//画面エフェクト描画処理
	void EffectAndUIDraw() override;											
};