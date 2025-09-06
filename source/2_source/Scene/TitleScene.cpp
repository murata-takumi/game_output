#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Scene/SceneComposition.h"
#include "Scene/TitleScene.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �V�[���J�n���̏���
/// </summary>
void
TitleScene::SceneStart()
{
	//�֐����㏑��
	SceneComposition::Instance().SetEffectAndUiDraw(std::bind(
		&TitleScene::EffectAndUIDraw, this));
	SceneComposition::Instance().SetModelDraw(std::bind(&TitleScene::ModelDraw, this));

	//���[�h����
	SceneComposition::Instance().SetCanInput(true);

	//�t�F�[�h�C����������񏈗�
	auto startFunc = [&]()
	{
		Dx12Wrapper::Instance().Fade(1.0f, 0.0f);
	};
	SceneComposition::Instance().ParallelProcess(startFunc);
}

/// <summary>
/// �X�V����
/// </summary>
void
TitleScene::Update()
{
	//���͂��X�V
	SceneComposition::Instance().InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && SceneComposition::Instance().GetCanInput())
	{
		//�J�n�{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//�Q�[���V�[���֑J��
			SceneComposition::Instance().ChangeScene(SceneNames::Play);

			return;
		}

		//�I���{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//����s�ɂ���
			SceneComposition::Instance().SetCanInput(false);

			auto exitFunc = ([&]()
				{
					//�t�F�[�h�A�E�g
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//�Q�[�����I��
					Application::Instance().ExitApp();

					return;
				}
			);
			SceneComposition::Instance().ParallelProcess(exitFunc);

			return;
		}
	}

	//�`�揈��
	SceneComposition::Instance().DrawUpdate();
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

/// <summary>
/// �G�t�F�N�g��̕`����s���֐�
/// </summary>
void
TitleScene::EffectAndUIDraw()
{

}