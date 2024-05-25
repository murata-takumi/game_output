#pragma once
#include "Application.h"

#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

#include <chrono>

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class PlayScene
{
public:
	PlayScene(							//�R���X�g���N�^
		Dx12Wrapper& dx12,
		InputManager& input,
		SoundManager& sound
	);
	~PlayScene();						//�f�X�g���N�^

	void Update();						//�X�V����

	void SceneStart();					//�V�[���J�n���̏���
	void SceneEnd();					//�V�[���I�����̏���

private:
	Application& _app;												//Application�C���X�^���X
	Dx12Wrapper& _dx12;												//Dx12Wrapper�C���X�^���X
	InputManager& _input;											//InputManager�C���X�^���X
	SoundManager& _sound;											//SoundManager�C���X�^���X

	vector<shared_ptr<FBXBase>> _actorAndObjs;						//FBXObject��FBXActor���܂Ƃ߂��x�N�g��
	shared_ptr<FBXActor> _actor;									//�A�N�^�[�C���X�^���X

	shared_ptr<Renderer> _renderer;									//Renderer�C���X�^���X
	shared_ptr<PeraRenderer> _pera;									//PeraRenderer�C���X�^���X

	XMVECTOR _direction;											//�v���C���[�A�J�����̐i�s�x�N�g��

	bool _isInLoad;													//���[�h���ł��邱�Ƃ������^���l

	LARGE_INTEGER _currentTime;										//�Q�[���̌��ݎ��Ԃ��i�[����l
	LARGE_INTEGER _updatedTime;										//�Q�[���̒��O�̎��Ԃ��i�[����l
	LARGE_INTEGER _beforeTime;										//�Q�[���̎��Ԃ��ꎞ�ۑ����Ă������߂̒l

	double _fps;													//1�b������ɉ�ʂ��؂�ւ���

	int _interval;													//�t���b�v�Ԋu

	void PeraDraw();												//�y���|���S���̕`�揈�����܂Ƃ߂��֐�

	void GameDraw();												//�Q�[����ʃ|���S���̕`�揈�����܂Ƃ߂��֐�

	//��̓����蔻�肪�Փ˂��Ă��邩�m�F����֐�
	bool CheckIntersect(map<string, float> col0, map<string, float> col1);		

	void DrawUpdate();												//�`��֐�

	void InputUpdate();												//���͍X�V�֐�

	void ModelDraw();												//���f���`�揈��
	void EffectAndUIDraw();											//��ʃG�t�F�N�g�`�揈��

	void UpdateFPS();												//FPS���X�V����֐�
};