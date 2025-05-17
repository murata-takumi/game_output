#pragma once
#include "Application.h"

#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"
#include "FBX/FBXActor.h"
#include "FBX/FBXBase.h"
#include "FBX/FBXObject.h"
#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Scene/BaseScene.h"

#include <chrono>

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class PlayScene : public BaseScene
{
public:
	//�R���X�g���N�^
	PlayScene();
	//�f�X�g���N�^
	~PlayScene();						

	//�X�V����
	void Update();						

	//�V�[���J�n���̏���
	void SceneStart();				
	//�V�[���I�����̏���
	void SceneEnd();					

protected:
	//FBXObject��FBXActor���܂Ƃ߂��x�N�g��
	map<string, shared_ptr<FBXBase>> _actorAndObjs;
	//�A�N�^�[�C���X�^���X
	shared_ptr<FBXActor> _actor;
	//���C���X�^���X
	shared_ptr<FBXObject> _ground;									

	//���񏈗��������X���b�h�̃x�N�g��
	vector<thread> _ths;

	//�v���C���[�A�J�����̐i�s�x�N�g��
	XMVECTOR _direction;							

	char* _currentFootObjName;

	//�I�u�W�F�N�g�𐶐�����֐�
	template<class className> void InstantiateObject(const wchar_t* path, string name,XMFLOAT3 size, XMFLOAT3 pos = XMFLOAT3(0.0f,0.0f,0.0f));												

	//OBB���m�̓����蔻����m�F����֐�
	bool CheckColliders(const BoxCollider& col1, const BoxCollider& col2);

	//OBB�Ƀx�N�g���������Ă��邩�m�F����֐�
	bool CheckColAndPoint(const BoxCollider& col, const XMVECTOR& point);

	//�������ɓ��e���ꂽ�����̒������擾����
	float LenOnSeparateAxis(const XMVECTOR& sep, const XMVECTOR& right, const XMVECTOR& up, const XMVECTOR& front = XMVectorZero());

	//���f���`�揈��
	void ModelDraw() override;												
	//��ʃG�t�F�N�g�`�揈��
	void EffectAndUIDraw() override;											
};