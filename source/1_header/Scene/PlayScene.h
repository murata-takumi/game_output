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
	//�R���X�g���N�^
	PlayScene();
	//�f�X�g���N�^
	~PlayScene();						

	void Update();						//�X�V����

	void SceneStart();					//�V�[���J�n���̏���
	void SceneEnd();					//�V�[���I�����̏���

private:
	Dx12Wrapper& _dx12;												//Dx12Wrapper�C���X�^���X

	vector<shared_ptr<FBXBase>> _actorAndObjs;						//FBXObject��FBXActor���܂Ƃ߂��x�N�g��
	shared_ptr<FBXActor> _actor;									//�A�N�^�[�C���X�^���X

	shared_ptr<PeraRenderer> _pera;									//PeraRenderer�C���X�^���X

	//���񏈗��������X���b�h�̃x�N�g��
	vector<thread> _ths;

	XMVECTOR _direction;											//�v���C���[�A�J�����̐i�s�x�N�g��

	bool _isInLoad;													//���[�h���ł��邱�Ƃ������^���l

	LARGE_INTEGER _currentTime;										//�Q�[���̌��ݎ��Ԃ��i�[����l
	LARGE_INTEGER _updatedTime;										//�Q�[���̒��O�̎��Ԃ��i�[����l
	LARGE_INTEGER _beforeTime;										//�Q�[���̎��Ԃ��ꎞ�ۑ����Ă������߂̒l

	double _fps;													//1�b������ɉ�ʂ��؂�ւ���

	int _interval;													//�t���b�v�Ԋu

	//�I�u�W�F�N�g�𐶐�����֐�
	template<class className> void InstantiateObject(const wchar_t* path);

	//�y���|���S���̕`�揈�����܂Ƃ߂��֐�
	void PeraDraw();	
	//�Q�[����ʃ|���S���̕`�揈�����܂Ƃ߂��֐�
	void GameDraw();												

	//��̓����蔻�肪�Փ˂��Ă��邩�m�F����֐�
	bool CheckIntersect(map<string, float> col0, map<string, float> col1);

	//�`��֐�
	void DrawUpdate();		
	//���͍X�V�֐�
	void InputUpdate();
	//FPS���X�V����֐�
	void FPSUpdate();

	//���f���`�揈��
	void ModelDraw();												
	//��ʃG�t�F�N�g�`�揈��
	void EffectAndUIDraw();											
};