#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// �����f�[�^���Ǘ�����N���X
/// </summary>
class SoundManager
{
private:

	unique_ptr<DirectX::AudioEngine> _audioEngine;		//�T�E���h�S�̂��Ǘ�

	unique_ptr<DirectX::SoundEffect> _buttonEffect;		//�{�^���������̌��ʉ���ǂݍ���

public:
	
	SoundManager();		//�R���X�g���N�^
	~SoundManager();	//�f�X�g���N�^

	void CallButton();	//�{�^���������̌��ʉ�
};