#pragma once
#include "Scene/IScene.h"

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class BoxCollider;
class collisionDetector;
class AssimpLoader;
class FBXActor;
class FBXBase;
class FBXComposition;
class FBXObject;
class EffectManager;
class ImGuiManager;
class OcTreeNode;
class OcTree;
class SceneComposition;
class Vector3;
class PlayScene : public IScene
{
public:
	//コンストラクタ
	PlayScene() = default;
	//デストラクタ
	~PlayScene() = default;

	//更新処理
	void Update() override;						

	//シーン開始時の処理
	void SceneStart() override;
	//シーン終了時の処理
	void SceneEnd() override;

protected:
	//FBXObjectとFBXActorをまとめたベクトル
	map<string, shared_ptr<FBXBase>> _actorAndObjs;

	//並列処理したいスレッドのベクトル
	vector<thread> _ths;

	//アクターインスタンス
	shared_ptr<FBXBase> _actor;
	//床インスタンス
	shared_ptr<FBXBase> _ground;

	//プレイヤー、カメラの進行ベクトル
	Vector3 _direction;

	//オブジェクトを生成する関数
	template<class className> void InstantiateObject(const wchar_t* path, string name,Vector3 size, Vector3 pos = Vector3(0.0f,0.0f,0.0f));

	//モデル描画処理
	void ModelDraw() override;												
	//画面エフェクト描画処理
	void EffectAndUIDraw() override;											
};