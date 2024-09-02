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
	:_fps(0.0f)
{
	//フリップ間隔を取得
	_interval = Application::Instance().GetInterval();
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
	//入力を更新
	InputUpdate();													

	//操作可能な場合入力を反映する
	if (!_isInLoad)													
	{
		_actor->SetOnGround(CheckColliders(*_actorAndObjs[0]->Collider(), *_actor->Collider()));
		ImGuiManager::Instance()._isCrossed = CheckColliders(*_actorAndObjs[0]->Collider(), *_actor->Collider());

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
				Dx12Wrapper::Instance().TranslateCoordinates(Y_VECTOR * 0.1f);
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
				Dx12Wrapper::Instance().TranslateCoordinates(Y_VECTOR * -1 * 0.1f);
			}
			else
			{
				Dx12Wrapper::Instance().ScalingCoordinates(90);
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
				_direction += Dx12Wrapper::Instance().GetRightVector();				//カメラに対し左方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Right)
			{
				_direction += Dx12Wrapper::Instance().GetRightVector() * -1;			//カメラに対し右方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Up)
			{
				_direction += Dx12Wrapper::Instance().GetXZVecEyeToTarget();			//カメラに対し奥方向にベクトル加算
			}
			if (InputManager::Instance().KeyState().Down)
			{
				_direction += Dx12Wrapper::Instance().GetXZVecEyeToTarget() * -1;		//カメラに対し手前方向にベクトル加算
			}

			if (XMVector3Length(_direction).m128_f32[0] > 0.0f)
			{
				_direction = XMVector3Normalize(_direction);		//移動ベクトルを正規化
				_actor->Translate(_direction);						//アクターを移動
				Dx12Wrapper::Instance().TranslateCoordinates(_direction);				//カメラも移動

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
	DrawUpdate();
}

/// <summary>
/// シーン開始時に実行する関数
/// </summary>
void
PlayScene::SceneStart()
{
	//カメラ位置をリセット
	Dx12Wrapper::Instance().ResetCoordinates(0,0);								

	//プレイヤーの移動ベクトルを初期化
	_direction = XMVectorZero();										

	//モデル読み込み処理
	auto f = [&]()
	{
		//ロード中だと示す
		_isInLoad = true;

		//スレッドに各オブジェクトの作成処理を追加
		InstantiateObject<FBXActor>(ACTOR_PATH);
		InstantiateObject<FBXObject>(GROUND_PATH);
		//InstantiateObject<FBXObject>(BALL_PATH);

		//スレッドが全て完了するまでここで待機
		for (auto& th : _ths)							
		{
			th.join();
		}

		//オブジェクトの中からアクターを探す
		for (const auto& object : _actorAndObjs)
		{
			if (_actor = dynamic_pointer_cast<FBXActor>(object))
			{
				break;
			}
		}

		//オブジェクトの中から床を探す
		for (const auto& object : _actorAndObjs)
		{
			if (_ground = dynamic_pointer_cast<FBXObject>(object))
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
			Dx12Wrapper::Instance().Fade(1.0f, 0.0f);										
		};
		ParallelProcess(startFunc);

		//エフェクト実行
		//Dx12Wrapper::Instance().Effect()->CallEffect();									
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

		object = make_shared<className>(Dx12Wrapper::Instance(), path);

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
	PeraRenderer::Instance().BeginPeraDraw();

	//背景を描画
	SpriteManager::Instance().BackGroundDraw();

	//ペラポリゴン用パイプラインをセット
	Dx12Wrapper::Instance().SetPipelines(							
		Renderer::Instance().GetRootSignature(),
		Renderer::Instance().GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	//ビュー・プロジェクション行列を適用
	Dx12Wrapper::Instance().SetScene();

	//モデル描画処理
	ModelDraw();								

	//ペラポリゴン描画後始末
	PeraRenderer::Instance().EndPeraDraw();						
}

/// <summary>
/// ゲーム画面描画処理
/// </summary>
void
PlayScene::GameDraw()
{
	//ゲーム画面描画準備
	Dx12Wrapper::Instance().BeginGameDraw();															

	//以下ゲーム画面用パイプラインをセット
	PeraRenderer::Instance().SetPeraPipelines();														

	//ペラポリゴン用VBVをセット
	Dx12Wrapper::Instance().CommandList()->IASetVertexBuffers(0, 1, PeraRenderer::Instance().PeraVBView());				
	//ペラポリゴンを構成する頂点を描画
	Dx12Wrapper::Instance().CommandList()->DrawInstanced(4, 1, 0, 0);									

	//グリッドを描画
	if (ImGuiManager::Instance().IsWriteGrid() && !_isInLoad)SpriteManager::Instance().GridDraw();		

	//ここにエフェクト上の描画処理
	EffectAndUIDraw();																

	//ゲーム画面描画後始末
	Dx12Wrapper::Instance().EndGameDraw();															

	//スワップチェーンのフリップ処理
	Dx12Wrapper::Instance().Swapchain()->Present(_interval, 0);										

	//グラフィックスメモリを設定
	SpriteManager::Instance().Commit();														
}

/// <summary>
/// 二つのOBBが衝突しているか確認する関数
/// </summary>
/// <param name="obb1"></param>
/// <param name="obb2"></param>
/// <returns>衝突しているかどうか</returns>
bool
PlayScene::CheckColliders(const BoxCollider& col1, const BoxCollider& col2)
{
	//各OBBの右、上、正面ベクトル
	XMVECTOR rightLen1 = col1._rightDir * col1._halfWidth;
	XMVECTOR upLen1 = col1._upDir * col1._halfHeight;
	XMVECTOR frontLen1 = col1._frontDir * col1._halfDepth;
	XMVECTOR rightLen2 = col2._rightDir * col2._halfWidth;
	XMVECTOR upLen2 = col2._upDir * col2._halfHeight;
	XMVECTOR frontLen2 = col2._frontDir * col2._halfDepth;

	//双方のOBBの中心を結ぶベクトル
	XMVECTOR vecBetCenter = col1._center - col2._center;

	//obb1_front
	float r = LenOnSeparateAxis(col1._frontDir, rightLen2, upLen2, frontLen2);
	float lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col1._frontDir ).m128_f32[0]);
	bool a = lenBetCenter > r + col1._halfDepth;
	ImGuiManager::Instance()._front1 = a;
	if (a)
	{
		return false;
	}

	//obb1_right
	r = LenOnSeparateAxis(col1._rightDir, rightLen2, upLen2, frontLen2);
	lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col1._rightDir).m128_f32[0]);
	a = lenBetCenter > r + col1._halfWidth;
	ImGuiManager::Instance()._right1 = a;
	if (a)
	{
		return false;
	}

	//obb1_up
	r = LenOnSeparateAxis(col1._upDir, rightLen2, upLen2, frontLen2);
	lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col1._upDir).m128_f32[0]);
	a = lenBetCenter > r + col1._halfHeight;
	ImGuiManager::Instance()._up1 = a;
	if (a)
	{
		return false;
	}

	//obb2_front
	r = LenOnSeparateAxis(col2._frontDir, rightLen1, upLen1, frontLen1);
	lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col2._frontDir).m128_f32[0]);
	a = lenBetCenter > r + col2._halfDepth;
	ImGuiManager::Instance()._front2 = a;
	if (a)
	{
		return false;
	}

	//obb2_right
	r = LenOnSeparateAxis(col2._rightDir, rightLen1, upLen1, frontLen1);
	lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col2._rightDir).m128_f32[0]);
	a = lenBetCenter > r + col2._halfWidth;
	ImGuiManager::Instance()._right2 = a;
	if (a)
	{
		return false;
	}

	//obb2_up
	r = LenOnSeparateAxis(col2._upDir, rightLen1, upLen1, frontLen1);
	lenBetCenter = fabs(XMVector3Dot(vecBetCenter,col2._upDir).m128_f32[0]);
	a = lenBetCenter > r + col2._halfHeight;
	ImGuiManager::Instance()._up2 = a;
	if (a)
	{
		return false;
	}

	return true;
}

/// <summary>
/// 分離軸に対し投影された各ベクトルの大きさの和を求める関数
/// </summary>
/// <param name="sep">分離軸</param>
/// <param name="right">右ベクトル</param>
/// <param name="up">上ベクトル</param>
/// <param name="front">正面ベクトル</param>
/// <returns>ベクトルの大きさの和</returns>
float 
PlayScene::LenOnSeparateAxis(const XMVECTOR& sep, const XMVECTOR& right, const XMVECTOR& up, const XMVECTOR& front)
{
	//分離軸と各ベクトルの内積を取り、その絶対値が投影された大きさとなる
	float r1 = fabs(XMVector3Dot(sep, right).m128_f32[0]);
	float r2 = fabs(XMVector3Dot(sep, up).m128_f32[0]);
	float r3 = fabs(XMVector3Dot(sep, front).m128_f32[0]);

	return r1 + r2 + r3;
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