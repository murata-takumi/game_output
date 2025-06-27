#pragma once
#include "Scene/IScene.h"

/// <summary>
/// �^�C�g���V�[�����Ǘ�����N���X
/// </summary>
class SceneComponent;
class TitleScene : public IScene
{
public:
	//�R���X�g���N�^
	TitleScene();
	//�f�X�g���N�^
	~TitleScene();					

	//�X�V����
	void Update() override;			

	//�V�[���J�n���̏���
	void SceneStart() override;		
	//�V�[���I�����̏���
	void SceneEnd() override;		

	//UI�`�揈��
	void ModelDraw() override;
	//�G�t�F�N�g�EUI��`�悷��֐�
	void EffectAndUIDraw();
protected:
	//�S�V�[���ɕK�v�ȋ@�\���܂Ƃ߂��N���X
	shared_ptr<SceneComposition> _sceneComp;
};