#pragma once
#include "Application.h"

/// <summary>
/// �e�V�[���I�u�W�F�N�g�Ɏ�������R���|�W�V�����N���X
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class SceneComposition
{
public:
	static SceneComposition& Instance();

	//���f���̕`����s���֐����b�p�[
	std::function<void()> _modelDraw;
	//�G�t�F�N�g�AUI�̕`����s���֐����b�p�[
	std::function<void()> _effectAndUiDraw;

	//���񏈗����s�����A���\�[�X��r�����䂷��ׂ̕ϐ�
	mutex _mtx;

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

	//�����_�����󂯎����񏈗����s���֐�
	void ParallelProcess(function<void(void)> func);
};