#include "Manager/SoundManager.h"

const wchar_t* BUTTON_PATH = L"Asset/sound/button.wav";
const wchar_t* COINGET_PATH = L"Asset/sound/coinget.wav";

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
/// �V���O���g����Ԃ�
/// </summary>
/// <returns>�V���O���g��</returns>
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
	InsertSound(BUTTON, BUTTON_PATH);
	InsertSound(COIN, COINGET_PATH);
}

/// <summary>
/// �{�^���������̌��ʉ��̍Đ�
/// </summary>
/// <param name="sounds">�Đ�������SE</param>
void
SoundManager::CallSound(Sounds sounds)
{
	_soundsMap[sounds]->Play();
}

/// <summary>
/// �A�z�z���BGM,SE��R�t������Ŋi�[����֐�
/// </summary>
/// <param name="sounds">�R�t�������񋓑�</param>
/// <param name="path">BGM,SE���i�[���Ă���p�X</param>
void
SoundManager::InsertSound(Sounds sounds, const wchar_t* path)
{
	_soundsMap[sounds] = make_unique<SoundEffect>(_audioEngine.get(), path);
}