#pragma once
#include "Includes.h"
#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// 音声データを管理するクラス
/// </summary>
class SoundManager
{
private:
	//サウンド全体を管理
	unique_ptr<DirectX::AudioEngine> _audioEngine;		
	//ボタン押下時の効果音を読み込む
	unique_ptr<DirectX::SoundEffect> _buttonEffect;		
	
	//コンストラクタ
	SoundManager();		
	//デストラクタ
	~SoundManager();	

public:
	//シングルトンを返す
	static SoundManager& Instance();

	//初期化を行う
	void Init();

	//ボタン押下時の効果音
	void CallButton();	
};