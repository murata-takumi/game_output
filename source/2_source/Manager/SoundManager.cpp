#include "Manager/SoundManager.h"

/// <summary>
/// �R���X�g���N�^
/// �����f�[�^�̓ǂݍ��݂��s��
/// </summary>
SoundManager::SoundManager()
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;												//�t���O��ݒ�

#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif // DEBUG
	_audioEngine = make_unique<AudioEngine>(eflags);												//AudioEngine�C���X�^���X��������

	_buttonEffect = make_unique<SoundEffect>(_audioEngine.get(), L"Asset/sound/button.wav");		//���ʉ��ǂݍ���
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
/// �{�^���������̌��ʉ��̍Đ�
/// </summary>
void
SoundManager::CallButton()
{
	_buttonEffect->Play();
}