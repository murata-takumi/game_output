#pragma once
#include "Scene/BaseScene.h"

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class BoxCollider;
class collisionDetector;
class AssimpLoader;
class FBXActor;
class FBXBase;
class FBXObject;
class EffectManager;
class ImGuiManager;
class OcTreeNode;
class OcTree;
class Vector3;
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
	Vector3 _direction;

	//オブジェクトを生成する関数
	template<class className> void InstantiateObject(const wchar_t* path, string name,Vector3 size, Vector3 pos = Vector3(0.0f,0.0f,0.0f));

	//モデル描画処理
	void ModelDraw() override;												
	//画面エフェクト描画処理
	void EffectAndUIDraw() override;											
};