#pragma once
#include "Scene/IScene.h"

/// <summary>
/// �Q�[���V�[�����Ǘ�����N���X
/// </summary>
class BoxCollider;
class collisionDetector;
class AssimpLoader;
class FBXActor;
class FBXBase;
class FBXComposition;
class FBXObject;
class EffectManager;
class ImGuiManager;
class OcTreeNode;
class OcTree;
class SceneComposition;
class Vector3;
class PlayScene : public IScene
{
public:
	//�R���X�g���N�^
	PlayScene() = default;
	//�f�X�g���N�^
	~PlayScene() = default;

	//�X�V����
	void Update() override;						

	//�V�[���J�n���̏���
	void SceneStart() override;
	//�V�[���I�����̏���
	void SceneEnd() override;

protected:
	//FBXObject��FBXActor���܂Ƃ߂��x�N�g��
	map<string, shared_ptr<FBXBase>> _actorAndObjs;

	//���񏈗��������X���b�h�̃x�N�g��
	vector<thread> _ths;

	//�A�N�^�[�C���X�^���X
	shared_ptr<FBXBase> _actor;
	//���C���X�^���X
	shared_ptr<FBXBase> _ground;

	//�v���C���[�A�J�����̐i�s�x�N�g��
	Vector3 _direction;

	//�I�u�W�F�N�g�𐶐�����֐�
	template<class className> void InstantiateObject(const wchar_t* path, string name,Vector3 size, Vector3 pos = Vector3(0.0f,0.0f,0.0f));

	//���f���`�揈��
	void ModelDraw() override;												
	//��ʃG�t�F�N�g�`�揈��
	void EffectAndUIDraw() override;											
};