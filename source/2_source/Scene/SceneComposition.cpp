#include "Functions.h"
#include "Includes.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Scene/SceneComposition.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// シーンを変更する関数
/// </summary>
/// <param name="name">変更先のシーンの名前</param>
void
SceneComposition::ChangeScene(SceneNames name)
{
	auto changeFunc = [&, name]()
	{
		//操作不可にする
		_canInput = false;

		//フェードアウト処理
		Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

		//シーンを遷移させる
		Application::Instance().ChangeScene(name);
	};
	//並列処理
	ParallelProcess(changeFunc);
}

/// <summary>
/// ペラポリゴンの描画処理をまとめた関数
/// </summary>
void
SceneComposition::PeraDraw()
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
	_modelDraw();

	//ペラポリゴン描画後始末
	PeraRenderer::Instance().EndPeraDraw();
}

/// <summary>
/// ゲーム画面描画処理
/// </summary>
void
SceneComposition::GameDraw()
{
	//ゲーム画面描画準備
	Dx12Wrapper::Instance().BeginGameDraw();

	//以下ペラポリゴン用コマンドリスト処理
	PeraRenderer::Instance().SetPeraPipelines();

	//ペラポリゴン用VBVをセット
	Dx12Wrapper::Instance().CommandList()->IASetVertexBuffers(0, 1, PeraRenderer::Instance().PeraVBView());
	//ペラポリゴンを構成する頂点を描画
	Dx12Wrapper::Instance().CommandList()->DrawInstanced(4, 1, 0, 0);

	//ここにエフェクト上の描画処理
	_effectAndUiDraw();

	//ゲーム画面描画後始末
	Dx12Wrapper::Instance().EndGameDraw();

	//スワップチェーンのフリップ処理
	Dx12Wrapper::Instance().Swapchain()->Present(Application::Instance().GetInterval(), 0);

	//グラフィックスメモリを設定
	SpriteManager::Instance().Commit();
}

/// <summary>
/// 各描画の更新をまとめて行う関数
/// </summary>
void
SceneComposition::DrawUpdate()
{
	//FPSを更新
	FPSUpdate();

	//ペラポリゴン描画処理
	PeraDraw();

	//ゲーム画面描画処理
	GameDraw();
}

/// <summary>
/// 入力を更新する関数
/// </summary>
void
SceneComposition::InputUpdate()
{
	//入力を更新
	InputManager::Instance().UpdateInput();
}

/// <summary>
/// 現在のフレームレートを更新する関数
/// </summary>
void
SceneComposition::FPSUpdate()
{
	//現在フレームの時間を取得
	QueryPerformanceCounter(&_currentTime);

	//前回FPS更新時との差分
	auto diff = GetLIntDiff(_currentTime, _updatedTime);
	//前フレームとの差分
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);

	//処理時間に余裕がある場合、待ち合わせを行う
	if (frameTime < FRAME_TIME)
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	//差分が1秒以上だった場合
	if (diff >= 1)
	{
		//1秒を差分で割り、FPSを取得
		_fps = 1 / frameTime;

		//FPS更新時間を更新
		_updatedTime = _currentTime;
	}

	//前フレームの時間を更新
	_beforeTime = _currentTime;
}

/// <summary>
/// ラムダ式を受け取り並列処理を行う関数
/// </summary>
/// <param name="func">並列に処理したいラムダ式</param>
void
SceneComposition::ParallelProcess(function<void(void)> func)
{
	//ラムダ式を処理するスレッドを作成
	thread th(func);
	//スレッドの管理を手放す
	th.detach();
}