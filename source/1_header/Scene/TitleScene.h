#pragma once
#include "Scene/BaseScene.h"

/// <summary>
/// �^�C�g���V�[�����Ǘ�����N���X
/// </summary>
class TitleScene : public BaseScene
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

protected:
};