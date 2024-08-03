#pragma once
#include "Includes.h"
#include <DirectXTK12-master/Inc/Audio.h>

/// <summary>
/// �����f�[�^���Ǘ�����N���X
/// </summary>
class SoundManager
{
private:
	//�T�E���h�S�̂��Ǘ�
	unique_ptr<DirectX::AudioEngine> _audioEngine;		
	//�{�^���������̌��ʉ���ǂݍ���
	unique_ptr<DirectX::SoundEffect> _buttonEffect;		
	
	//�R���X�g���N�^
	SoundManager();		
	//�f�X�g���N�^
	~SoundManager();	

public:
	//�V���O���g����Ԃ�
	static SoundManager& Instance();

	//���������s��
	void Init();

	//�{�^���������̌��ʉ�
	void CallButton();	
};