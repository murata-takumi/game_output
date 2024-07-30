#include "Manager/SoundManager.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
SoundManager::SoundManager()
{

}

/// <summary>
/// �f�X�g���N�^
/// AudioEngine�C���X�^���X�̃��Z�b�g���s��
/// </summary>
SoundManager::~SoundManager()
{
	_audioEngine->Reset();
}

/// <summary>
/// �ÓI�ϐ���Ԃ�
/// </summary>
/// <returns>�ÓI�ϐ�</returns>
SoundManager&
SoundManager::Instance()
{
	static SoundManager instance;
	return instance;
}

void
SoundManager::Init()
{
	//�t���O��ݒ�
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;												

#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif // DEBUG

	//AudioEngine�C���X�^���X��������
	_audioEngine = make_unique<AudioEngine>(eflags);												

	//���ʉ��ǂݍ���
	_buttonEffect = make_unique<SoundEffect>(_audioEngine.get(), L"Asset/sound/button.wav");		
}

/// <summary>
/// �{�^���������̌��ʉ��̍Đ�
/// </summary>
void
SoundManager::CallButton()
{
	_buttonEffect->Play();
}