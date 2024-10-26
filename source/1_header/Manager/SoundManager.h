#pragma once
#include "Includes.h"
#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// SE�ƕR�Â��񋓑�
/// </summary>
enum Sounds
{
	BUTTON,
	COIN,
	LENGTH,
};

/// <summary>
/// �����f�[�^���Ǘ�����N���X
/// </summary>
class SoundManager
{
private:
	//�T�E���h�S�̂��Ǘ�
	unique_ptr<DirectX::AudioEngine> _audioEngine;
	
	//�T�E���h���܂Ƃ߂��A�z�z��
	unique_ptr<SoundEffect> _soundsMap[LENGTH];

	//�R���X�g���N�^
	SoundManager();		
	//�f�X�g���N�^
	~SoundManager();	

	//�A�z�z��ɗv�f��ǉ�����
	void InsertSound(Sounds sounds, const wchar_t* path);

public:
	//�V���O���g����Ԃ�
	static SoundManager& Instance();

	//���������s��
	void Init();

	//���ʉ��Ăяo��
	void CallSound(Sounds sounds);
};