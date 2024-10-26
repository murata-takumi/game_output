#pragma once
#include "Includes.h"
#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// SEと紐づく列挙体
/// </summary>
enum Sounds
{
	BUTTON,
	COIN,
	LENGTH,
};

/// <summary>
/// 音声データを管理するクラス
/// </summary>
class SoundManager
{
private:
	//サウンド全体を管理
	unique_ptr<DirectX::AudioEngine> _audioEngine;
	
	//サウンドをまとめた連想配列
	unique_ptr<SoundEffect> _soundsMap[LENGTH];

	//コンストラクタ
	SoundManager();		
	//デストラクタ
	~SoundManager();	

	//連想配列に要素を追加する
	void InsertSound(Sounds sounds, const wchar_t* path);

public:
	//シングルトンを返す
	static SoundManager& Instance();

	//初期化を行う
	void Init();

	//効果音呼び出し
	void CallSound(Sounds sounds);
};