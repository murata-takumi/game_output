#pragma once
#include "Functions.h"
#include "Scene/PlayScene.h"
#include "Includes.h"

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
//コインの初期座標
const Vector3 COIN_INIT_POS = Vector3(0.0f, 0.0f, 100.0f);
//箱の初期座標
//const Vector3 BOX_INIT_POS = Vector3(600.0f, -200.0f, 0.0f);
const Vector3 BOX_INIT_POS = Vector3(300.0f, 0.0f, 0.0f);

//オブジェクトの名前一覧
const string ACTOR_NAME = "Actor";
const string GROUND_NAME = "Ground";
const string COIN_NAME = "Coin";
const string BOX_NAME = "Box";

/// <summary>
/// コンストラクタ
/// </summary>
PlayScene::PlayScene()
	:BaseScene()
{

}

/// <summary>
/// デストラクタ
/// </summary>
PlayScene::~PlayScene()
{
	
}

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	//カメラ位置をリセット
	Dx12Wrapper::Instance().ResetCoordinates(0,0);	

	_node = new OcTreeNode(Bounds(Vector3(0, 0, 0), Vector3(1000, 1000, 1000)), 4);

	//プレイヤーの移動ベクトルを初期化
	_direction = XMVectorZero();

	//乗る対象となるオブジェクトの名前も初期化
	_currentFootObjName = new char[100];
	strcpy(_currentFootObjName, GROUND_NAME.c_str());

	//モデル読み込み処理
	auto f = [&]()
	{
		//ロード中だと示す
		BaseScene::_canInput = false;

		//スレッドに各オブジェクトの作成処理を追加
		InstantiateObject<FBXActor>(ACTOR_PATH,ACTOR_NAME,ACTOR_COLL_SIZE);
		InstantiateObject<FBXObject>(GROUND_PATH,GROUND_NAME,GROUND_COLL_SIZE);
		//InstantiateObject<FBXObject>(COIN_PATH,COIN_NAME,COIN_COLL_SIZE,COIN_INIT_POS);
		InstantiateObject<FBXObject>(BOX_PATH,BOX_NAME,BOX_COLL_SIZE,BOX_INIT_POS);

		//スレッドが全て完了するまでここで待機
		for (auto& th : _ths)							
		{
			th.join();
		}

		if (!_node->AddObject(dynamic_pointer_cast<FBXObject>(_actorAndObjs[GROUND_NAME])))
		{
			assert(0);
		}
		if (!_node->AddObject(dynamic_pointer_cast<FBXObject>(_actorAndObjs[BOX_NAME])))
		{
			assert(0);
		}

		//オブジェクトの中からアクターを探す
		for (const auto& object : _actorAndObjs)
		{
			if (_actor = dynamic_pointer_cast<FBXActor>(object.second))
			{
				//コールバックを設定
				_actor->_isOnGround = [&](const XMVECTOR& vec)
				{
					return CollisionDetector::Instance().CheckColAndPoint(
						*_actorAndObjs[_currentFootObjName]->Collider().get(),
						vec
					);
				};
				break;
			}
		}

		ImGuiManager::Instance().SetActor(_actorAndObjs);

		ImGuiManager::Instance().AddLabelAndFloat("BoxX", _actorAndObjs[BOX_NAME]->Collider()->Center().X());
		ImGuiManager::Instance().AddLabelAndFloat("BoxY", _actorAndObjs[BOX_NAME]->Collider()->Center().Y());
		ImGuiManager::Instance().AddLabelAndFloat("BoxZ", _actorAndObjs[BOX_NAME]->Collider()->Center().Z());

		//ロード完了
		BaseScene::_canInput = true;

		//フェードイン処理を並列処理
		auto startFunc = [&]()
		{
			Dx12Wrapper::Instance().Fade(1.0f, 0.0f);										
		};
		BaseScene::ParallelProcess(startFunc);

		//エフェクト実行
		//Dx12Wrapper::Instance().Effect()->CallEffect();									
	};
	//上記の読み込み処理を通常の描画処理と並行して行う
	BaseScene::ParallelProcess(f);
}

/// <summary>
/// 更新処理
/// </summary>
void
PlayScene::Update()
{
	//入力を更新
	BaseScene::InputUpdate();

	//操作可能な場合入力を反映する
	if (BaseScene::_canInput)
	{
		ImGuiManager::Instance().AddText(0, _currentFootObjName);

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

			//当たり判定をチェック
			CollisionDetector::Instance().CheckColAndCol(*_actorAndObjs[BOX_NAME]->Collider(), *_actor->Collider());
			CollisionDetector::Instance().CheckColAndCol(*_actorAndObjs[GROUND_NAME]->Collider(), *_actor->Collider());

			//アクターを移動
			_actor->Translate(_direction);

			//移動ベクトルを初期化
			_direction = XMVectorZero();

			//カメラを移動
			Dx12Wrapper::Instance().SetCoordinatesCenter(_actor->Pos());

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
	BaseScene::DrawUpdate();
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
template<class className>
void
PlayScene::InstantiateObject(const wchar_t* path,string name, Vector3 size, Vector3 pos)
{
	auto f = [=]() {
		shared_ptr<className> object;

		object = make_shared<className>(path, name, size, pos);

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
	//プレイヤーが読み込み完了している場合FBXBaseクラスを基盤とするオブジェクト全般に対し描画・更新処理を行う
	if (BaseScene::_canInput)
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
	if (!BaseScene::_canInput)
	{
		//ロード画面を描画
		SpriteManager::Instance().LoadingDraw();	
	}
	else								
	{
		//グリッドを描画
		if (ImGuiManager::Instance().IsWriteGrid())SpriteManager::Instance().GridDraw(_actorAndObjs);

		//SceneからImGuiにFPSを渡す
		ImGuiManager::Instance().SetFPS(_fps);	
		//ImGuiのウィンドウを描画
		ImGuiManager::Instance().ImGuiDraw();	
	}
}