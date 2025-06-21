#pragma once
#include "Scene/BaseScene.h"

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class BoxCollider;
class collisionDetector;
class AssimpLoader;
class FBXActor;
class FBXBase;
class FBXObject;
class EffectManager;
class ImGuiManager;
class OcTreeNode;
class OcTree;
class Vector3;
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
	Vector3 _direction;

	//�I�u�W�F�N�g�𐶐�����֐�
	template<class className> void InstantiateObject(const wchar_t* path, string name,Vector3 size, Vector3 pos = Vector3(0.0f,0.0f,0.0f));

	//���f���`�揈��
	void ModelDraw() override;												
	//��ʃG�t�F�N�g�`�揈��
	void EffectAndUIDraw() override;											
};