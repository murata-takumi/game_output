#include "Manager/InputManager.h"
#include "Manager/SoundManager.h"
#include "Manager/SpriteManager.h"
#include "Scene/SceneComposition.h"
#include "Scene/TitleScene.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
TitleScene::TitleScene():IScene()
{
	//�V�[���̋��ʋ@�\��������
	_sceneComp = make_shared<SceneComposition>();
	//�֐����㏑��
	_sceneComp->_effectAndUiDraw = std::bind(&TitleScene::EffectAndUIDraw, this);
	_sceneComp->_modelDraw = std::bind(&TitleScene::ModelDraw, this);
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

}

/// <summary>
/// �X�V����
/// </summary>
void
TitleScene::Update()
{
	//���͂��X�V
	_sceneComp->InputUpdate();

	if (InputManager::Instance().MouseTracker().leftButton ==
		Mouse::ButtonStateTracker::PRESSED && _sceneComp->_canInput)
	{
		//�J�n�{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnStart())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//�Q�[���V�[���֑J��
			_sceneComp->ChangeScene(SceneNames::Play);

			return;
		}

		//�I���{�^���̏�ō��N���b�N
		if (SpriteManager::Instance().TitleIsOnExit())
		{
			//���ʉ�
			SoundManager::Instance().CallSound(Sounds::BUTTON);

			//����s�ɂ���
			_sceneComp->_canInput = false;

			auto exitFunc = ([&]()
				{
					//�t�F�[�h�A�E�g
					Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

					//�Q�[�����I��
					Application::Instance().ExitApp();

					return;
				}
			);
			_sceneComp->ParallelProcess(exitFunc);

			return;
		}
	}

	//�`�揈��
	_sceneComp->DrawUpdate();
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