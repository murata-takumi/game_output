#pragma once
#include "Scene/IScene.h"

#include <queue>

/// <summary>
/// ゲームシーンを管理するクラス
/// </summary>
class BoxCollider;
class collisionDetector;
class AssimpLoader;
class FbxActor;
class IFbx;
class FbxComposition;
class FbxObject;
class EffectManager;
class ImGuiManager;
class OcTreeNode;
class OcTree;
class SceneComposition;
class SphereCollider;
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
	queue<pair<string, shared_ptr<IFbx>>> que;

	//FbxObjectとFbxActorをまとめたベクトル
	map<string, shared_ptr<IFbx>> _actorAndObjs;

	//並列処理したいスレッドのベクトル
	vector<thread> _ths;

	//アクターインスタンス
	shared_ptr<FbxActor> _actor;
	//床インスタンス
	shared_ptr<IFbx> _ground;

	//プレイヤー、カメラの進行ベクトル
	Vector3 _direction;

	//オブジェクトを生成する関数
	template<class fbxType> void InstantiateObject(
		const wchar_t* path, string name,Vector3 size, 
		Vector3 pos = Vector3(0.0f,0.0f,0.0f),ColliderType colType = ColliderType::Box);

	//モデル描画処理
	void ModelDraw() override;												
	//画面エフェクト描画処理
	void EffectAndUIDraw() override;											
};