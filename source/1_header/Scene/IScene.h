#pragma once
#include "Application.h"

#include <chrono>

/// <summary>
/// �e�V�[���I�u�W�F�N�g�̃C���^�[�t�F�C�X
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class IScene
{
	friend ImGuiManager;

public:

	//�R���X�g���N�^
	IScene() = default;
	//�f�X�g���N�^
	~IScene() = default;

	//�X�V����
	virtual void Update() = 0;				
	//�V�[���J�n���̏���
	virtual void SceneStart() = 0;		
	//�V�[���I�����̏���
	virtual void SceneEnd() = 0;
protected:

	//���f����`�悷�邽�߂̃N���X
	virtual void ModelDraw() = 0;

	//�G�t�F�N�g�EUI��`�悷��֐�
	virtual void EffectAndUIDraw() = 0;
};