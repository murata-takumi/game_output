#pragma once
#include "Functions.h"
#include "Scene/PlayScene.h"
#include "Includes.h"

const wchar_t* ACTOR_PATH = L"Asset/model/Unitychan.fbx";		//モデルデータが格納されているファイルパス
const wchar_t* GROUND_PATH = L"Asset/model/G.fbx";				//地面データが格納されているファイルパス
const wchar_t* BALL_PATH = L"Asset/model/Ball BLEND.fbx";		//サッカーボールのデータが格納されているファイルパス
const float FRAME_TIME = 1.0f / 60.0f;							//更新回数を60FPSに固定する際の1フレームの秒数

/// <summary>
/// コンストラクタ
/// </summary>
PlayScene::PlayScene()
	:_dx12(Dx12Wrapper::Instance()), _fps(0.0f)
{
	//フリップ間隔を取得
	_interval = Application::Instance().GetInterval();

	//パイプラインを初期化
	Renderer::Instance().Init();
	_pera.reset(new PeraRenderer(_dx12));
}

/// <summary>
/// デストラクタ
/// </summary>
PlayScene::~PlayScene()
{
	
}

/// <summary>
/// 更新処理
/// </summary>
void
PlayScene::Update()
{
	InputUpdate();													//入力を更新

	if (!_isInLoad)													//操作可能な場合入力を反映する
	{
		auto cols0 = _actorAndObjs[0]->Collider()->BoundValues();
		auto cols1 = _actor->Collider()->BoundValues();

		_actor->SetOnGround(CheckIntersect(cols0, cols1));

		//WASDキーの入力に応じて視点を移動させる
		//注視点を中心に視点を回転
		if (InputManager::Instance().KeyState().A)
		{
			_dx12.RotateCoordinates(Degree::Azimth, 45);
		}
		if (InputManager::Instance().KeyState().D)
		{
			_dx12.RotateCoordinates(Degree::Azimth, -45);
		}
		if (InputManager::Instance().KeyState().W)
		{
			_dx12.RotateCoordinates(Degree::Elevation, 30);
		}
		if (InputManager::Instance().KeyState().S)
		{
			_dx12.RotateCoordinates(Degree::Elevation, -30);
		}

		//Q･Eキーを押した時は拡大・縮小
		//左シフトキーを押している場合はカメラを上下に平行移動させる
		if (InputManager::Instance().KeyState().Q)
		{
			if (InputManager::Instance().KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(-90);
			}
		}
		if (InputManager::Instance().KeyState().E)
		{
			if (InputManager::Instance().KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * -1 * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(90);
			}
		}
		//アクターが移動可能かどうか
		if (ImGuiManager::Instance().CanActorTrans())
		{
			if ((InputManager::Instance().KeyState().Left ||
				InputManager::Instance().KeyState().Right ||
				InputManager::Instance().KeyState().Up    ||
				InputManager::Instance().KeyState().Down) &&
				!_actor->IsAnimationEqual(RUN00_F)
				)
			{
				_actor->SetAnimationNode(RUN00_F);
			}

			if (InputManager::Instance().KeyState().Left)
			{
				_direction += _dx12.GetRightVector();				//カメラに対し左方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Right)
			{
				_direction += _dx12.GetRightVector() * -1;			//カメラに対し右方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Up)
			{
				_direction += _dx12.GetXZVecEyeToTarget();			//カメラに対し奥方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Down)
			{
				_direction += _dx12.GetXZVecEyeToTarget() * -1;		//カメラに対し手前方向にベクトル加算
			}

			if (XMVector3Length(_direction).m128_f32[0] > 0.0f)
			{
				_direction = XMVector3Normalize(_direction);		//移動ベクトルを正規化
				_actor->Translate(_direction);						//アクターを移動
				_dx12.TranslateCoordinates(_direction);				//カメラも移動

				_direction = XMVectorZero();						//移動ベクトルを初期化
			}

			if ((!InputManager::Instance().KeyState().Left	&&
				!InputManager::Instance().KeyState().Right	&&
				!InputManager::Instance().KeyState().Up		&&
				!InputManager::Instance().KeyState().Down)    &&
				!_actor->IsAnimationEqual(WAIT00)
				)
			{
				_actor->SetAnimationNode(WAIT00);
			}

			if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::Space) &&
				!_actor->IsAnimationEqual(JUMP00))
			{
				_actor->SetAnimationNode(JUMP00);
			}
		}

		//キーパッドの入力に応じてカメラを正面・側面・上面に配置
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad1))
		{
			_dx12.ResetCoordinates(0, 0);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad3))
		{
			_dx12.ResetCoordinates(XM_PIDIV2, 0);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad5))
		{
			auto isPers = _dx12.Perspective();
			_dx12.SetPersOrOrtho(!isPers);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad7))
		{
			_dx12.ResetCoordinates(0, XM_PIDIV2);
		}
		if (InputManager::Instance().KeyTracker().IsKeyPressed(Keyboard::NumPad9))
		{
			_dx12.ResetCoordinates(0, -1 * XM_PIDIV2);
		}
	}

	//描画処理
	DrawUpdate();
}

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	//カメラ位置をリセット
	_dx12.ResetCoordinates(0,0);								

	//プレイヤーの移動ベクトルを初期化
	_direction = XMVectorZero();										

	//モデル読み込み処理
	auto f = [&]()
	{
		//ロード中だと示す
		_isInLoad = true;

		//スレッドにアクターの作成処理を追加
		InstantiateObject<FBXActor>(ACTOR_PATH);
		InstantiateObject<FBXObject>(GROUND_PATH);
		InstantiateObject<FBXObject>(BALL_PATH);

		//スレッドが全て完了するまでここで待機
		for (auto& th : _ths)							
		{
			th.join();
		}

		for (const auto& object : _actorAndObjs)
		{
			if (_actor = dynamic_pointer_cast<FBXActor>(object))
			{
				break;
			}
		}

		//アクターをImGuiManager側に渡す
		ImGuiManager::Instance().SetActor(_actorAndObjs);							
		//アクターをSpriteManager側に渡す
		SpriteManager::Instance().SetActors(_actorAndObjs);						

		//ロード完了
		_isInLoad = false;		

		//フェードイン処理を並列処理
		auto startFunc = [&]()
		{
			_dx12.Fade(1.0f, 0.0f);										
		};
		ParallelProcess(startFunc);

		//エフェクト実行
		//_dx12.Effect()->CallEffect();									
	};
	//上記の読み込み処理を通常の描画処理と並行して行う
	ParallelProcess(f);													
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
template<class className>
void
PlayScene::InstantiateObject(const wchar_t* path)
{
	lock_guard<mutex> lock(_mtx);

	auto f = [=]() {
		shared_ptr<className> object;

		object = make_shared<className>(_dx12, path);

		_actorAndObjs.emplace_back(object);
	};

	//スレッドにアクターの作成処理を追加
	_ths.emplace_back(f);
}

/// <summary>
/// 入力を更新する関数
/// </summary>
void
PlayScene::InputUpdate()
{
	InputManager::Instance().UpdateInput();
}

/// <summary>
/// 現在のフレームレートを更新する関数
/// </summary>
void
PlayScene::FPSUpdate()
{
	//現在フレームの時間を取得
	QueryPerformanceCounter(&_currentTime);							

	//前回FPS更新時との時間の差分
	auto diff = GetLIntDiff(_currentTime, _updatedTime);			
	//前フレームとの時間の差分
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		

	//処理時間に余裕がある場合、処理を待ち合わせる
	if (frameTime < FRAME_TIME)										
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	//前回FPS更新から1秒以上たった時
	if (diff >= 1)													
	{
		//1秒を前フレームとの時間の差分で割り、FPSを取得
		_fps = 1 / frameTime;
		_updatedTime = _currentTime;
	}

	//前フレームの時間を更新
	_beforeTime = _currentTime;										
}

/// <summary>
/// 各描画の更新をまとめて行う関数
/// </summary>
void
PlayScene::DrawUpdate()
{
	//FPSを更新
	FPSUpdate();			

	//ペラポリゴン描画処理
	PeraDraw();				

	//ゲーム画面描画処理
	GameDraw();				
}

/// <summary>
/// ペラポリゴンの描画処理をまとめた関数
/// </summary>
void
PlayScene::PeraDraw()
{
	//ペラポリゴン描画準備
	_pera->BeginPeraDraw();

	//背景を描画
	SpriteManager::Instance().BackGroundDraw();

	//ペラポリゴン用パイプラインをセット
	_dx12.SetPipelines(							
		Renderer::Instance().GetRootSignature(),
		Renderer::Instance().GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	//ビュー・プロジェクション行列を適用
	_dx12.SetScene();

	//モデル描画処理
	ModelDraw();								

	//ペラポリゴン描画後始末
	_pera->EndPeraDraw();						
}

/// <summary>
/// ゲーム画面描画処理
/// </summary>
void
PlayScene::GameDraw()
{
	//ゲーム画面描画準備
	_dx12.BeginGameDraw();															

	//以下ゲーム画面用パイプラインをセット
	_pera->SetPeraPipelines();														

	//ペラポリゴン用VBVをセット
	_dx12.CommandList()->IASetVertexBuffers(0, 1, _pera->PeraVBView());				
	//ペラポリゴンを構成する頂点を描画
	_dx12.CommandList()->DrawInstanced(4, 1, 0, 0);									

	//グリッドを描画
	if (ImGuiManager::Instance().IsWriteGrid() && !_isInLoad)SpriteManager::Instance().GridDraw();		

	//ここにエフェクト上の描画処理
	EffectAndUIDraw();																

	//ゲーム画面描画後始末
	_dx12.EndGameDraw();															

	//スワップチェーンのフリップ処理
	_dx12.Swapchain()->Present(_interval, 0);										

	//グラフィックスメモリを設定
	SpriteManager::Instance().Commit();														
}

/// <summary>
/// 二つの当たり判定が衝突しているか確認する関数
/// </summary>
/// <param name="col0">当たり判定その1</param>
/// <param name="col1">当たり判定その2</param>
/// <returns>衝突しているかどうか</returns>
bool 
PlayScene::CheckIntersect(map<string, float> col0, map<string, float> col1)
{
	//col0がGround、col1がActor
	bool intersects[6] =
	{
		(col0[BOX_COL_X_MIN] - col1[BOX_COL_X_MAX]) > FLT_EPSILON,
		(col1[BOX_COL_X_MIN] - col0[BOX_COL_X_MAX]) > FLT_EPSILON,
		(col0[BOX_COL_Y_MAX] - col1[BOX_COL_Y_MIN]) > FLT_EPSILON,
		(col1[BOX_COL_Y_MAX] - col0[BOX_COL_Y_MIN]) > FLT_EPSILON,
		(col0[BOX_COL_Z_MAX] - col1[BOX_COL_Z_MIN]) > FLT_EPSILON,
		(col1[BOX_COL_Z_MAX] - col0[BOX_COL_Z_MIN]) > FLT_EPSILON
	};

	ImGuiManager::Instance().SetActorColBool(intersects);

	if ((col0[BOX_COL_X_MIN] - col1[BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col1[BOX_COL_X_MIN] - col0[BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col0[BOX_COL_Y_MAX] - col1[BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col1[BOX_COL_Y_MAX] - col0[BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col0[BOX_COL_Z_MIN] - col1[BOX_COL_Z_MAX]) > FLT_EPSILON)return false;
	if ((col1[BOX_COL_Z_MIN] - col0[BOX_COL_Z_MAX]) > FLT_EPSILON)return false;

	return true;
}

/// <summary>
/// モデル描画を行う関数
/// </summary>
void
PlayScene::ModelDraw()
{
	//プレイヤーが読み込み完了している場合FBXBaseクラスを基盤とするオブジェクト全般に対し描画・更新処理を行う
	if (!_isInLoad)									
	{
		for (const auto& item : _actorAndObjs)
		{
			if (item != nullptr)
			{
				item->Draw();
				item->Update();
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
	if (_isInLoad)						
	{
		//ロード画面を描画
		SpriteManager::Instance().LoadingDraw();	
	}
	else								
	{
		//SceneからImGuiにFPSを渡す
		ImGuiManager::Instance().SetFPS(_fps);	
		//ImGuiのウィンドウを描画
		ImGuiManager::Instance().ImGuiDraw();		
	}
}