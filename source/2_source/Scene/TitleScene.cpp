#include "Scene/TitleScene.h"

#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Wrapper/Dx12Wrapper.h"

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
/// �V�[���J�n���̏���
/// </summary>
void
TitleScene::SceneStart()
{
	BaseScene::SceneStart();
}

/// <summary>
/// �X�V����
/// </summary>
void
TitleScene::Update()
{
	//���͂��X�V
	BaseScene::InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && BaseScene::_canInput)
	{
		//�J�n�{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//�Q�[���V�[���֑J��
			BaseScene::ChangeScene(SceneNames::Play);

			return;
		}

		//�I���{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//����s�ɂ���
			BaseScene::_canInput = false;

			auto exitFunc = ([&]()
				{
					//�t�F�[�h�A�E�g
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//�Q�[�����I��
					Application::Instance().ExitApp();

					return;
				}
			);
			BaseScene::ParallelProcess(exitFunc);

			return;
		}
	}

	//�`�揈��
	BaseScene::DrawUpdate();
}

/// <summary>
/// �V�[���I�����̏���
/// </summary>
void
TitleScene::SceneEnd()
{

}

/// <summary>
/// UI�`�揈��
/// </summary>
void
TitleScene::ModelDraw()
{
	SpriteManager::Instance().ButtonDraw();
}