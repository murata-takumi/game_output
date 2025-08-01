#pragma once
#include "Functions.h"
#include "Includes.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "Collider/SphereCollider.h"
#include "FBX/AssimpLoader.h"
#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FbxActor.h"
#include "FBX/IFbx.h"
#include "FBX/FbxObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "OcTree/OcTreeNode.h"
#include "OcTree/OcTree.h"
#include "Scene/PlayScene.h"
#include "Scene/SceneComposition.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

//プレイヤーデータが格納されているファイルパス
const wchar_t* ACTOR_PATH = L"Asset/model/Unitychan.fbx";		
//地面データが格納されているファイルパス
const wchar_t* GROUND_PATH = L"Asset/model/Ground.fbx";				
//コインのデータが格納されているファイルパス
const wchar_t* COIN_PATH = L"Asset/model/Coin.fbx";			

//箱のデータが格納されているファイルパス
const wchar_t* BOX_PATH = L"Asset/model/Box.fbx";				

//プレイヤーの当たり判定の大きさ
const Vector3 ACTOR_COLL_SIZE = Vector3(60.0f, 170.0f, 60.0f);
//地面の当たり判定の大きさ
const Vector3 GROUND_COLL_SIZE = Vector3(1000.0f, 1.0f, 1000.0f);
//コインの当たり判定の大きさ
const Vector3 COIN_COLL_SIZE = Vector3(25.0f, 25.0f, 25.0f);
//箱の当たり判定
const Vector3 BOX_COLL_SIZE = Vector3(200.0f, 200.0f, 200.0f);
//プレイヤーの初期座標
const Vector3 ACTOR_INIT_POS = Vector3(0.0f, 0.0f, 0.0f);
//地面の初期座標
const Vector3 GROUND_INIT_POS = Vector3(0.0f, -1.0f, 0.0f);
//コインの初期座標
const Vector3 COIN_INIT_POS = Vector3(0.0f, 0.0f, 100.0f);
//箱の初期座標
//const Vector3 BOX_INIT_POS = Vector3(100.0f, -150.0f, 0.0f);
const Vector3 BOX_INIT_POS = Vector3(500.0f, -500.0f, 0.0f);
const Vector3 BOX_INIT_POS_1 = Vector3(-600.0f, 200.0f, -600.0f);
const Vector3 BOX_INIT_POS_2 = Vector3(0.0f, -200.0f, 600.0f);
const Vector3 BOX_INIT_POS_3 = Vector3(0.0f, -200.0f, -600.0f);

//オブジェクトの名前一覧
const string ACTOR_NAME = "Actor";
const string GROUND_NAME = "Ground";
const string COIN_NAME = "Coin";
const string BOX_NAME = "Box";
const string BOX_NAME_1 = "Box1";
const string BOX_NAME_2 = "Box2";
const string BOX_NAME_3 = "Box3";

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	//関数も上書き
	SceneComposition::Instance()._effectAndUiDraw = std::bind(
		&PlayScene::EffectAndUIDraw,
		this);
	SceneComposition::Instance()._modelDraw = std::bind(&PlayScene::ModelDraw, this);

	//カメラ位置をリセット
	Dx12Wrapper::Instance().ResetCoordinates(0,0);	

	//プレイヤーの移動ベクトルを初期化
	_direction = XMVectorZero();

	//モデル読み込み処理
	auto f = [&]()
	{
		//ロード中だと示す
		SceneComposition::Instance()._canInput = false;

		//スレッドに各オブジェクトの作成処理を追加
		InstantiateObject<FbxActor>(
			ACTOR_PATH,ACTOR_NAME,ACTOR_COLL_SIZE,ACTOR_INIT_POS,ColliderType::Box);
		InstantiateObject<FbxObject>(
			GROUND_PATH,GROUND_NAME,GROUND_COLL_SIZE, GROUND_INIT_POS, ColliderType::Box);
		InstantiateObject<FbxObject>(
			COIN_PATH,COIN_NAME,COIN_COLL_SIZE,COIN_INIT_POS, ColliderType::Sphere);
		InstantiateObject<FbxObject>(
			BOX_PATH,BOX_NAME,BOX_COLL_SIZE,BOX_INIT_POS,ColliderType::Box);
		//InstantiateObject<FbxObject>(BOX_PATH,BOX_NAME_1,BOX_COLL_SIZE,BOX_INIT_POS_1);
		//InstantiateObject<FbxObject>(BOX_PATH,BOX_NAME_2,BOX_COLL_SIZE,BOX_INIT_POS_2);
		//InstantiateObject<FbxObject>(BOX_PATH,BOX_NAME_3,BOX_COLL_SIZE,BOX_INIT_POS_3);

		//スレッドが全て完了するまでここで待機
		for (auto& th : _ths)							
		{
			th.join();
		}

		//八分木ノードにオブジェクトを追加
		if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(
			_actorAndObjs[GROUND_NAME])))
		{
			assert(0);
		}
		if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(
			_actorAndObjs[COIN_NAME])))
		{
			assert(0);
		}
		if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(
			_actorAndObjs[BOX_NAME])))
		{
			assert(0);
		}
		/*if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(_actorAndObjs[BOX_NAME_1])))
		{
			assert(0);
		}
		if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(_actorAndObjs[BOX_NAME_2])))
		{
			assert(0);
		}
		if (!OcTree::Instance().AddObject(dynamic_pointer_cast<FbxObject>(_actorAndObjs[BOX_NAME_3])))
		{
			assert(0);
		}*/

		//オブジェクトの中からアクターを探す
		for (const auto& object : _actorAndObjs)
		{
			if (_actor = dynamic_pointer_cast<FbxActor>(object.second))
			{
				//コールバックを設定
				_actor->_isOnGround = [&](const XMVECTOR& vec)
				{
					//アクターの近くにあるオブジェクトを取得し当たり判定をチェック
					auto objsNearby = OcTree::Instance().Get(_actor->GetColForGround());
					auto ret = false;
					for (auto& obj : objsNearby)
					{
						//if (CollisionDetector::Instance().GetLengthBetweenColAndPos(
						//	*obj->Collider(),
						//	XMVectorSet(0, 1, 0, 0),
						//	vec
						//) < 0.0f)
						//if(CollisionDetector::Instance().CheckColAndVec(
						//	*obj->Collider(), 
						//	vec,
						//	vec - XMVectorSet(0,85.0f,0,0)))
						if (CollisionDetector::Instance().CheckColAndPoint(
							obj->Collider(),
							vec))
						//if(CollisionDetector::Instance().CheckContinuousCollisionDetection(
						//	obj->Collider(),
						//	XMVectorSet(0,1,0,0),
						//	vec,
						//	-45.0f * 9.8f
						//))
						//if(CollisionDetector::Instance().CheckColAndVec(
						//	*obj->Collider(),
						//	vec) <= 0.0f
						//)
						{
							ret = true;
							break;
						}
					}

					return ret;
				};
				break;
			}
		}

		ImGuiManager::Instance().SetActor(_actorAndObjs);

		//ロード完了
		SceneComposition::Instance()._canInput = true;

		//フェードイン処理を並列処理
		auto startFunc = [&]()
		{
			Dx12Wrapper::Instance().Fade(1.0f, 0.0f);										
		};
		SceneComposition::Instance().ParallelProcess(startFunc);

		//エフェクト実行
		//Dx12Wrapper::Instance().Effect()->CallEffect();									
	};
	//上記の読み込み処理を通常の描画処理と並行して行う
	SceneComposition::Instance().ParallelProcess(f);
}

/// <summary>
/// 更新処理
/// </summary>
void
PlayScene::Update()
{
	//入力を更新
	SceneComposition::Instance().InputUpdate();

	//操作可能な場合入力を反映する
	if (SceneComposition::Instance()._canInput)
	{
		//コインとアクターが接触したらコインを削除
		//if (_actorAndObjs[COIN_NAME] && CheckColAndCol(*_actorAndObjs[COIN_NAME]->Collider(), *_actor->Collider()))
		//{
		//	_actorAndObjs.erase(COIN_NAME);

		//	SoundManager::Instance().CallSound(Sounds::COIN);
		//}

		//WASDキーの入力に応じて視点を移動させる
		//注視点を中心に視点を回転
		if (InputManager::Instance().KeyState().A)
		{
			Dx12Wrapper::Instance().RotateCoordinates(Degree::Azimth, 45);
		}
		if (InputManager::Instance().KeyState().D)
		{
			Dx12Wrapper::Instance().RotateCoordinates(Degree::Azimth, -45);
		}
		if (InputManager::Instance().KeyState().W)
		{
			Dx12Wrapper::Instance().RotateCoordinates(Degree::Elevation, 30);
		}
		if (InputManager::Instance().KeyState().S)
		{
			Dx12Wrapper::Instance().RotateCoordinates(Degree::Elevation, -30);
		}

		//Q･Eキーを押した時は拡大・縮小
		//左シフトキーを押している場合はカメラを上下に平行移動させる
		if (InputManager::Instance().KeyState().Q)
		{
			if (InputManager::Instance().KeyState().LeftShift)
			{
				//Dx12Wrapper::Instance().SetCoordinatesCenter(Y_VECTOR * 0.1f);
			}
			else
			{
				Dx12Wrapper::Instance().ScalingCoordinates(-90);
			}
		}
		if (InputManager::Instance().KeyState().E)
		{
			if (InputManager::Instance().KeyState().LeftShift)
			{
				//Dx12Wrapper::Instance().SetCoordinatesCenter(Y_VECTOR * -1 * 0.1f);
			}
			else
			{
				Dx12Wrapper::Instance().ScalingCoordinates(90);
			}
		}

		//アクターが移動可能かどうか
		if (ImGuiManager::Instance().CanActorTrans())
		{
			//カメラに対し入力方向にベクトル加算
			if (InputManager::Instance().KeyState().Left)
			{
				_direction += Dx12Wrapper::Instance().GetRightVector();
			}
			if (InputManager::Instance().KeyState().Right)
			{
				_direction += Dx12Wrapper::Instance().GetRightVector() * -1;
			}
			if (InputManager::Instance().KeyState().Up)
			{
				_direction += Dx12Wrapper::Instance().GetXZVecEyeToTarget();
			}
			if (InputManager::Instance().KeyState().Down)
			{
				_direction += Dx12Wrapper::Instance().GetXZVecEyeToTarget() * -1;
			}

			//移動ベクトルを正規化
			_direction = XMVector3Normalize(_direction);

			//入力処理
			_actor->OnKeyPressed(_direction);

			//移動ベクトルを初期化
			_direction = XMVectorZero();

			//カメラを移動
			Dx12Wrapper::Instance().SetCoordinatesCenter(*_actor->Collider()->Center());

			//スペースキーが押されたらジャンプ処理
			if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::Space) && _actor->GetOnGround())
			{
				_actor->SetAnimationNode(JUMP00);
			}
		}

		//キーパッドの入力に応じてカメラを正面・側面・上面に配置
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad1))
		{
			Dx12Wrapper::Instance().ResetCoordinates(0, 0);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad3))
		{
			Dx12Wrapper::Instance().ResetCoordinates(XM_PIDIV2, 0);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad5))
		{
			auto isPers = Dx12Wrapper::Instance().Perspective();
			Dx12Wrapper::Instance().SetPersOrOrtho(!isPers);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad7))
		{
			Dx12Wrapper::Instance().ResetCoordinates(0, XM_PIDIV2);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad9))
		{
			Dx12Wrapper::Instance().ResetCoordinates(0, -1 * XM_PIDIV2);
		}
	}

	//描画処理
	SceneComposition::Instance().DrawUpdate();
}

/// <summary>
/// シーン遷移時に実行する関数
/// </summary>
void
PlayScene::SceneEnd()
{
	//オブジェクトをまとめて削除
	_actorAndObjs.clear();
	//アクターも初期化
	_actor = nullptr;					

	//ImGuiで表示されているアニメーション名を初期化
	ImGuiManager::Instance().ResetAnimStr();		
}

/// <summary>
/// オブジェクトを生成する関数
/// </summary>
/// <typeparam name="className">オブジェクトのクラス</typeparam>
/// <param name="path">オブジェクトデータを格納しているパス</param>
/// <param name="size">オブジェクトの当たり判定のサイズ</param>
template<class fbxType>
void
PlayScene::InstantiateObject(
	const wchar_t* path,string name, Vector3 size, Vector3 pos, ColliderType colType)
{
	auto f = [=]() {
		shared_ptr<IFbx> object;

		object = make_shared<fbxType>();
		object->Init(path, name, size, pos,colType);

		_actorAndObjs[name] = object;
	};

	//スレッドにアクターの作成処理を追加
	_ths.emplace_back(f);
}

/// <summary>
/// モデル描画を行う関数
/// </summary>
void
PlayScene::ModelDraw()
{
	//プレイヤーが読み込み完了している場合IFbxクラスを基盤とするオブジェクト全般に対し描画・更新処理を行う
	if (SceneComposition::Instance()._canInput)
	{
		//トゥーンテクスチャも描画する
		Renderer::Instance().Draw();

		for (const auto& item : _actorAndObjs)
		{
			if (item.second != nullptr)
			{
				item.second->Draw();
				item.second->Update();
			}
		}
	}
}

/// <summary>
/// エフェクト上の描画を行う関数
/// </summary>
void
PlayScene::EffectAndUIDraw()
{
	//ロード中の場合
	if (!SceneComposition::Instance()._canInput)
	{
		//ロード画面を描画
		SpriteManager::Instance().LoadingDraw();	
	}
	else								
	{
		//グリッドを描画
		if (ImGuiManager::Instance().IsWriteGrid())SpriteManager::Instance().GridDraw(_actorAndObjs);

		//SceneからImGuiにFPSを渡す
		ImGuiManager::Instance().SetFPS(SceneComposition::Instance()._fps);
		//ImGuiのウィンドウを描画
		ImGuiManager::Instance().ImGuiDraw();	
	}
}