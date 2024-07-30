#include "Manager/SoundManager.h"

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
/// 静的変数を返す
/// </summary>
/// <returns>静的変数</returns>
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
	_buttonEffect = make_unique<SoundEffect>(_audioEngine.get(), L"Asset/sound/button.wav");		
}

/// <summary>
/// ボタン押下時の効果音の再生
/// </summary>
void
SoundManager::CallButton()
{
	_buttonEffect->Play();
}