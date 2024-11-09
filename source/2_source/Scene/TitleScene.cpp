#include "Scene/TitleScene.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
TitleScene::TitleScene():BaseScene()
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
TitleScene::~TitleScene()
{

}

/// <summary>
/// �X�V����
/// </summary>
void
TitleScene::Update()
{
	//���͂��X�V
	InputUpdate();											


	if (InputManager::Instance().MouseTracker().leftButton == 
		Mouse::ButtonStateTracker::PRESSED && _isPlaying)
	{
		//�J�n�{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnStart())				
		{
			//���ʉ�
			SoundManager::Instance().CallSound(BUTTON);							

			//�Q�[���V�[���֑J��
			ChangeScene(SceneNames::Play);					

			return;
		}

		//�I���{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnEnd())					
		{
			//���ʉ�
			SoundManager::Instance().CallSound(BUTTON);							

			//����s�ɂ���
			_isPlaying = false;								

			auto exitFunc = ([&]()
				{
					//�t�F�[�h�A�E�g
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);					

					//�Q�[�����I��
					Application::Instance().ExitApp();							

					return;
				}
			);
			ParallelProcess(exitFunc);

			return;
		}
	}

	//�`�揈��
	DrawUpdate();	
}

/// <summary>
/// �V�[���J�n���̏���
/// </summary>
void
TitleScene::SceneStart()
{
	BaseScene::SceneStart();
}

/// <summary>
/// �V�[���I�����̏���
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// �w�i��`�悷��֐�
/// </summary>
void
TitleScene::BackGroundDraw()
{
	//�w�i��`��
	BaseScene::BackGroundDraw();	
}