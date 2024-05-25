#include "Functions.h"
#include "Scene/PlayScene.h"
#include "Constant.h"

const XMVECTOR Y_VECTOR = XMVectorSet(0.0f,90.0f,0.0f,0.0f);	//Y軸ベクトル
const wchar_t* ACTOR_PATH = L"Asset/model/Unitychan.fbx";		//モデルデータが格納されているファイルパス
const wchar_t* GROUND_PATH = L"Asset/model/G.fbx";				//地面データが格納されているファイルパス
const float FRAME_TIME = 1.0f / 60.0f;							//更新回数を60FPSに固定する際の1フレームの秒数

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="input">InputManagerインスタンス</param>
/// <param name="sound">SoundManagerインスタンス</param>
PlayScene::PlayScene(
	Dx12Wrapper& dx12,
	InputManager& input,
	SoundManager& sound
)
	:_dx12(dx12), _input(input), _sound(sound), _app(Application::Instance()), _fps(0.0f)
{
	_interval = _app.GetInterval();															//フリップ間隔を取得

	_renderer.reset(new Renderer(_dx12));
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
		auto cols1 = _actorAndObjs[1]->Collider()->BoundValues();

		_actor->SetOnGround(CheckIntersect(cols0, cols1));

		//WASDキーの入力に応じて視点を移動させる
		//注視点を中心に視点を回転
		if (_input.KeyState().A)
		{
			_dx12.RotateCoordinates(Degree::Azimth, 45);
		}
		if (_input.KeyState().D)
		{
			_dx12.RotateCoordinates(Degree::Azimth, -45);
		}
		if (_input.KeyState().W)
		{
			_dx12.RotateCoordinates(Degree::Elevation, 30);
		}
		if (_input.KeyState().S)
		{
			_dx12.RotateCoordinates(Degree::Elevation, -30);
		}

		//Q･Eキーを押した時は拡大・縮小
		//左シフトキーを押している場合はカメラを上下に平行移動させる
		if (_input.KeyState().Q)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(-90);
			}
		}
		if (_input.KeyState().E)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * -1 * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(90);
			}
		}
		//アクターが移動可能かどうか
		if (_dx12.ImGui()->CanActorTrans())
		{
			if ((_input.KeyState().Left ||
				_input.KeyState().Right ||
				_input.KeyState().Up    ||
				_input.KeyState().Down) &&
				!_actor->IsAnimationEqual(RUN00_F)
				)
			{
				_actor->SetAnimationNode(RUN00_F);
			}

			if (_input.KeyState().Left)
			{
				_direction += _dx12.GetRightVector();				//カメラに対し左方向にベクトル加算
			}
			if (_input.KeyState().Right)
			{
				_direction += _dx12.GetRightVector() * -1;			//カメラに対し右方向にベクトル加算
			}
			if (_input.KeyState().Up)
			{
				_direction += _dx12.GetXZVecEyeToTarget();			//カメラに対し奥方向にベクトル加算
			}
			if (_input.KeyState().Down)
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

			if ((!_input.KeyState().Left	&&
				!_input.KeyState().Right	&&
				!_input.KeyState().Up		&&
				!_input.KeyState().Down)    &&
				!_actor->IsAnimationEqual(WAIT00)
				)
			{
				_actor->SetAnimationNode(WAIT00);
			}

			if (_input.KeyTracker().IsKeyPressed(Keyboard::Space) &&
				!_actor->IsAnimationEqual(JUMP00))
			{
				_actor->SetAnimationNode(JUMP00);
			}
		}
		else
		{

		}

		//キーパッドの入力に応じてカメラを正面・側面・上面に配置
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad1))
		{
			_dx12.ResetCoordinates(0, 0);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad3))
		{
			_dx12.ResetCoordinates(XM_PIDIV2, 0);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad5))
		{
			auto isPers = _dx12.Perspective();
			_dx12.SetPersOrOrtho(!isPers);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad7))
		{
			_dx12.ResetCoordinates(0, XM_PIDIV2);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad9))
		{
			_dx12.ResetCoordinates(0, -1 * XM_PIDIV2);
		}
	}

	DrawUpdate();												//描画処理
}

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	_dx12.ResetCoordinates(0,0);								//カメラ位置をリセット

	_direction = XMVectorZero();										//移動ベクトルを初期化

	//モデル読み込み処理
	auto f = [&]()
	{
		_isInLoad = true;												//ロード中だと示す

		vector<thread> ths;												//並列処理したいスレッドのベクトル
		shared_ptr<FBXObject> ground;

		ths.emplace_back
		(	
			[&](){
				lock_guard lock(_mtx);

				_actor = make_shared<FBXActor>(_dx12, ACTOR_PATH);		//アクターを作成
			}
		);
		ths.emplace_back
		(
			[&]() {
				lock_guard lock(_mtx);

				ground = make_shared<FBXObject>(_dx12, GROUND_PATH);	//地面を作成
			}
		);

		for (auto& th : ths)											//スレッドの実行完了待ち
		{
			th.join();
		}

		_actorAndObjs.emplace_back(ground);
		_actorAndObjs.emplace_back(_actor);

		_dx12.ImGui()->SetActor(_actorAndObjs);							//アクターをImGuiManager側に渡す
		_dx12.Sprite()->SetActors(_actorAndObjs);						//アクターをSpriteManager側に渡す

		_isInLoad = false;												//ロード完了

		auto startFunc = [&]()
		{
			_dx12.Fade(1.0f, 0.0f);										//フェードイン処理
		};
		ParallelProcess(startFunc);										//上記の処理を並列処理する

		//_dx12.Effect()->CallEffect();									//エフェクト実行
	};
	ParallelProcess(f);													//モデル読み込み処理を通常の描画処理と並行して行う
}

/// <summary>
/// シーン遷移時に実行する関数
/// </summary>
void
PlayScene::SceneEnd()
{
	_actorAndObjs.clear();				//オブジェクトをまとめて削除
	_actor = nullptr;					//アクターも初期化

	_dx12.ImGui()->ResetAnimStr();		//ImGuiで表示されているアニメーション名を初期化
}

/// <summary>
/// 入力を更新する関数
/// </summary>
void
PlayScene::InputUpdate()
{
	_input.UpdateInput();									//入力を更新
}

/// <summary>
/// 現在のフレームレートを更新する関数
/// </summary>
void
PlayScene::UpdateFPS()
{
	QueryPerformanceCounter(&_currentTime);							//現在フレームの時間を取得

	auto diff = GetLIntDiff(_currentTime, _updatedTime);			//前回FPS更新時との差分
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		//前フレームとの差分

	if (frameTime < FRAME_TIME)										//処理時間に余裕がある場合、待ち合わせを行う
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	if (diff >= 1)													//差分が1秒以上だった場合
	{
		_fps = 1 / frameTime;										//1秒を差分で割り、FPSを取得

		_updatedTime = _currentTime;								//FPS更新時間を更新
	}

	_beforeTime = _currentTime;										//前フレームの時間を更新
}

/// <summary>
/// 各描画の更新をまとめて行う関数
/// </summary>
void
PlayScene::DrawUpdate()
{
	UpdateFPS();			//FPSを更新

	PeraDraw();				//ペラポリゴン描画処理

	GameDraw();				//ゲーム画面描画処理
}

/// <summary>
/// ペラポリゴンの描画処理をまとめた関数
/// </summary>
void
PlayScene::PeraDraw()
{
	_pera->BeginPeraDraw();						//ペラポリゴン描画準備

	_dx12.SetPipelines(							//ペラポリゴン用パイプラインをセット
		_renderer->GetRootSignature(),
		_renderer->GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	_dx12.SetScene();							//ビュー・プロジェクション行列を適用

	ModelDraw();								//モデル描画処理

	_pera->EndPeraDraw();						//ペラポリゴン描画後始末
}

/// <summary>
/// ゲーム画面描画処理
/// </summary>
void
PlayScene::GameDraw()
{
	_dx12.BeginGameDraw();															//ゲーム画面描画準備

	_pera->SetPeraPipelines();														//以下ペラポリゴン用コマンドリスト処理

	_dx12.CommandList()->IASetVertexBuffers(0, 1, _pera->PeraVBView());				//ペラポリゴン用VBVをセット
	_dx12.CommandList()->DrawInstanced(4, 1, 0, 0);									//ペラポリゴンを構成する頂点を描画

	if (_dx12.ImGui()->IsWriteGrid() && !_isInLoad)_dx12.Sprite()->GridDraw();		//グリッドを描画

	EffectAndUIDraw();																//ここにエフェクト上の描画処理

	_dx12.EndGameDraw();															//ゲーム画面描画後始末

	_dx12.Swapchain()->Present(_interval, 0);										//スワップチェーンのフリップ処理

	_dx12.Sprite()->Commit();														//グラフィックスメモリを設定
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
	if ((col0[Constant::BOX_COL_X_MIN] - col1[Constant::BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_X_MIN] - col0[Constant::BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col0[Constant::BOX_COL_Y_MAX] - col1[Constant::BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_Y_MAX] - col0[Constant::BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col0[Constant::BOX_COL_Z_MAX] - col1[Constant::BOX_COL_Z_MIN]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_Z_MAX] - col0[Constant::BOX_COL_Z_MIN]) > FLT_EPSILON)return false;

	return true;
}

/// <summary>
/// モデル描画を行う関数
/// </summary>
void
PlayScene::ModelDraw()
{
	if (!_isInLoad)									//プレイヤーが読み込み完了している場合描画処理
	{
		for (const auto& item : _actorAndObjs)		//FBXBaseクラスを基盤とするオブジェクト全般に対し描画・更新処理を行う
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
	if (_isInLoad)						//ロード中の場合
	{
		_dx12.Sprite()->LoadingDraw();	//ロード画面を描画
	}
	else								//操作可能な場合
	{
		_dx12.ImGui()->SetFPS(_fps);	//SceneからImGuiにデータを渡す
		_dx12.ImGui()->ImGuiDraw();		//ImGuiのウィンドウを描画
	}
}