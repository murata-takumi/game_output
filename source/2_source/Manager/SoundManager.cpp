#include "Manager/SoundManager.h"

const wchar_t* BUTTON_PATH = L"Asset/sound/button.wav";
const wchar_t* COINGET_PATH = L"Asset/sound/coinget.wav";

/// <summary>
/// コンストラクタ
/// </summary>
SoundManager::SoundManager()
{

}

/// <summary>
/// デストラクタ
/// AudioEngineインスタンスのリセットを行う
/// </summary>
SoundManager::~SoundManager()
{
	_audioEngine->Reset();
}

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
SoundManager&
SoundManager::Instance()
{
	static SoundManager instance;
	return instance;
}

void
SoundManager::Init()
{
	//フラグを設定
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;												

#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif // DEBUG

	//AudioEngineインスタンスを初期化
	_audioEngine = make_unique<AudioEngine>(eflags);												

	//効果音読み込み
	InsertSound(BUTTON, BUTTON_PATH);
	InsertSound(COIN, COINGET_PATH);
}

/// <summary>
/// ボタン押下時の効果音の再生
/// </summary>
/// <param name="sounds">再生したいSE</param>
void
SoundManager::CallSound(Sounds sounds)
{
	_soundsMap[sounds]->Play();
}

/// <summary>
/// 連想配列にBGM,SEを紐付けた上で格納する関数
/// </summary>
/// <param name="sounds">紐付けたい列挙体</param>
/// <param name="path">BGM,SEを格納しているパス</param>
void
SoundManager::InsertSound(Sounds sounds, const wchar_t* path)
{
	_soundsMap[sounds] = make_unique<SoundEffect>(_audioEngine.get(), path);
}