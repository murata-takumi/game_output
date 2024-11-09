#pragma once
#include "Application.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

#include <chrono>

/// <summary>
/// �e�V�[���I�u�W�F�N�g�̊�N���X
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class BaseScene
{
	friend ImGuiManager;

public:

	//�R���X�g���N�^
	BaseScene();
	//�f�X�g���N�^
	virtual ~BaseScene();

	//�X�V����
	virtual void Update() = 0;				
	//�V�[���J�n���̏���
	virtual void SceneStart();		
	//�V�[���I�����̏���
	virtual void SceneEnd();																	
protected:
	//���[�h���ł��邱�Ƃ������^���l
	static bool _isLoading;									
	//����\�ł��邱�Ƃ������^���l
	static bool _isPlaying;									

	//�Q�[���̌��ݎ��Ԃ��i�[����l
	static LARGE_INTEGER _currentTime;						
	//�Q�[���̒��O�̎��Ԃ��i�[����l
	static LARGE_INTEGER _updatedTime;						
	//�Q�[���̎��Ԃ��ꎞ�ۑ����Ă������߂̒l
	static LARGE_INTEGER _beforeTime;						

	//1�b������ɉ�ʂ��؂�ւ���
	double _fps;											

	//�t���b�v�Ԋu
	int _interval;											

	//�V�[���J�ڂ���ׂ̊֐�
	void ChangeScene(SceneNames name);						

	//�w�i��`�悷��֐�
	virtual void BackGroundDraw();							

	virtual void ModelDraw();

	//�G�t�F�N�g�EUI��`�悷��֐�
	virtual void EffectAndUIDraw();							

	//�y���|���S���̕`�揈�����܂Ƃ߂��֐�
	void PeraDraw();										

	//�Q�[����ʃ|���S���̕`�揈�����܂Ƃ߂��֐�
	void GameDraw();										

	//�`��֐�
	void DrawUpdate();										
	//���͍X�V�֐�
	void InputUpdate();			
	//FPS���X�V����֐�
	void UpdateFPS();										

	//�����_�����󂯎����񏈗����s���֐�
	void ParallelProcess(function<void(void)> func);		
};