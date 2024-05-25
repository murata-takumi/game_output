#pragma once
#include "Application.h"

#include <EffekseerRendererDX12/Effekseer.h>
#include <EffekseerRendererDX12/EffekseerRendererDX12.h>

using namespace EffekseerRendererDX12;

/// <summary>
/// エフェクトを管理するクラス
/// </summary>
class Dx12Wrapper;
class EffectManager
{
private:

	Dx12Wrapper& _dx12;																			//Dx12Wrapperインスタンス

	EffekseerRenderer::RendererRef _efkRenderer = nullptr;										//エフェクトレンダラー

	Effekseer::ManagerRef _efkManager = nullptr;												//エフェクトマネージャー

	//EffekseerをDX12で使用できるようにするための変数
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> _efkMemoryPool = nullptr;		//メモリプール

	Effekseer::RefPtr<EffekseerRenderer::CommandList> _efkCmdList = nullptr;					//コマンドリスト

	//エフェクト再生に必要なもの
	Effekseer::EffectRef _effect = nullptr;														//エフェクト本体（エフェクトファイルに対応）

	Effekseer::Handle _efkHandle;																//エフェクトハンドル（再生中のエフェクトに対応）

public:

	EffectManager(Dx12Wrapper& dx12);		//コンストラクタ

	void CallEffect();						//エフェクトを開始する関数

	void Draw();							//エフェクトを更新する関数
};