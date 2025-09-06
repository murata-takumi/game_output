#pragma once
#include "Application.h"

/// <summary>
/// �e�V�[���I�u�W�F�N�g�Ɏ�������R���|�W�V�����N���X
/// </summary>
class Dx12Wrapper;
class FbxActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class SceneComposition
{
public:
	static SceneComposition& Instance();

	//���񏈗����s�����A���\�[�X��r�����䂷��ׂ̕ϐ�
	mutex _mtx;

	//FPS�̃Q�b�^�[
	double GetFps()const;

	//���͂��󂯕t���邩�ǂ���
	bool GetCanInput()const;

	//���͏�Ԃ�ݒ肷��
	void SetCanInput(bool canInput);

	//�V�[���J�ڂ���ׂ̊֐�
	void ChangeScene(SceneNames name);

	//�y���|���S���̕`�揈�����܂Ƃ߂��֐�
	void PeraDraw();
	//�Q�[����ʃ|���S���̕`�揈�����܂Ƃ߂��֐�
	void GameDraw();

	//�`��֐�
	void DrawUpdate();
	//���͍X�V�֐�
	void InputUpdate();
	//�t���[�����[�g���X�V����֐�
	void FPSUpdate();

	//���f���`����s���֐���ݒ肷��
	void SetModelDraw(function<void()> modelDraw);

	//�G�t�F�N�g�AUI�`����s���֐���ݒ肷��
	void SetEffectAndUiDraw(function<void()> effectAndUiDraw);

	//�����_�����󂯎����񏈗����s���֐�
	void ParallelProcess(function<void(void)> func);

private:
	//���f���̕`����s���֐����b�p�[
	function<void()> _modelDraw;
	//�G�t�F�N�g�AUI�̕`����s���֐����b�p�[
	function<void()> _effectAndUiDraw;

	//�Q�[���̌��ݎ��Ԃ��i�[����l
	LARGE_INTEGER _currentTime;
	//�Q�[���̒��O�̎��Ԃ��i�[����l
	LARGE_INTEGER _updatedTime;
	//�Q�[���̎��Ԃ��ꎞ�ۑ����Ă������߂̒l
	LARGE_INTEGER _beforeTime;

	//1�b������ɉ�ʂ��؂�ւ���
	double _fps;

	//���[�h���ł��邱�Ƃ������^���l
	bool _canInput;

};