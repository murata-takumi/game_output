#include "Functions.h"
#include "Includes.h"

#include "Scene/BaseScene.h"

//�e�V�[���Ń��[�h�����ǂ��������ʂ���^���l
bool BaseScene::_canInput = false;

//���t���[���̎���
LARGE_INTEGER BaseScene::_currentTime;	
//FPS�X�V���̎���
LARGE_INTEGER BaseScene::_updatedTime;	
//�O�t���[���̎���
LARGE_INTEGER BaseScene::_beforeTime;	

double BaseScene::_fps;

/// <summary>
/// �R���X�g���N�^
/// </summary>
BaseScene::BaseScene()
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
BaseScene::~BaseScene()
{

}

/// <summary>
/// �V�[���J�n���Ɏ��s���鏈��
/// </summary>
void
BaseScene::SceneStart()
{
	auto startFunc = [&]()
	{	//�t�F�[�h�C������
		Dx12Wrapper::Instance().Fade(1.0f, 0.0f);		
	};
	//��L�̏�������񏈗�����
	ParallelProcess(startFunc);		

	//����\�ɂ���
	_canInput = true;
}

/// <summary>
/// �V�[���I�����Ɏ��s���鏈��
/// </summary>
void
BaseScene::SceneEnd()
{

}

/// <summary>
/// �V�[����ύX����֐�
/// </summary>
/// <param name="name">�ύX��̃V�[���̖��O</param>
void
BaseScene::ChangeScene(SceneNames name)
{
	auto changeFunc = [&, name]()
	{
		//����s�ɂ���
		_canInput = false;

		//�t�F�[�h�A�E�g����
		Dx12Wrapper::Instance().Fade(0.0f, 1.0f);

		//�V�[����J�ڂ�����
		Application::Instance().ChangeScene(name);
	};
	//���񏈗�
	ParallelProcess(changeFunc);	
}

void
BaseScene::ModelDraw()
{

}

void
BaseScene::EffectAndUIDraw()
{

}

/// <summary>
/// �y���|���S���̕`�揈�����܂Ƃ߂��֐�
/// </summary>
void
BaseScene::PeraDraw()
{
	//�y���|���S���`�揀��
	PeraRenderer::Instance().BeginPeraDraw();	

	//�w�i��`��
	SpriteManager::Instance().BackGroundDraw();

	//�y���|���S���p�p�C�v���C�����Z�b�g
	Dx12Wrapper::Instance().SetPipelines(							
		Renderer::Instance().GetRootSignature(),
		Renderer::Instance().GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	//�r���[�E�v���W�F�N�V�����s���K�p
	Dx12Wrapper::Instance().SetScene();							

	//���f���`�揈��
	ModelDraw();								

	//�y���|���S���`���n��
	PeraRenderer::Instance().EndPeraDraw();				
}

/// <summary>
/// �Q�[����ʕ`�揈��
/// </summary>
void
BaseScene::GameDraw()
{
	//�Q�[����ʕ`�揀��
	Dx12Wrapper::Instance().BeginGameDraw();															

	//�ȉ��y���|���S���p�R�}���h���X�g����
	PeraRenderer::Instance().SetPeraPipelines();												

	//�y���|���S���pVBV���Z�b�g
	Dx12Wrapper::Instance().CommandList()->IASetVertexBuffers(0, 1, PeraRenderer::Instance().PeraVBView());		
	//�y���|���S�����\�����钸�_��`��
	Dx12Wrapper::Instance().CommandList()->DrawInstanced(4, 1, 0, 0);									

	//�����ɃG�t�F�N�g��̕`�揈��
	EffectAndUIDraw();																

	//�Q�[����ʕ`���n��
	Dx12Wrapper::Instance().EndGameDraw();															

	//�X���b�v�`�F�[���̃t���b�v����
	Dx12Wrapper::Instance().Swapchain()->Present(Application::Instance().GetInterval(), 0);

	//�O���t�B�b�N�X��������ݒ�
	SpriteManager::Instance().Commit();														
}

/// <summary>
/// �e�`��̍X�V���܂Ƃ߂čs���֐�
/// </summary>
void
BaseScene::DrawUpdate()
{
	//FPS���X�V
	FPSUpdate();

	//�y���|���S���`�揈��
	PeraDraw();				

	//�Q�[����ʕ`�揈��
	GameDraw();				
}

/// <summary>
/// ���͂��X�V����֐�
/// </summary>
void
BaseScene::InputUpdate()
{
	//���͂��X�V
	InputManager::Instance().UpdateInput();																	
}

/// <summary>
/// ���݂̃t���[�����[�g���X�V����֐�
/// </summary>
void
BaseScene::FPSUpdate()
{
	//���݃t���[���̎��Ԃ��擾
	QueryPerformanceCounter(&_currentTime);							

	//�O��FPS�X�V���Ƃ̍���
	auto diff = GetLIntDiff(_currentTime,_updatedTime);				
	//�O�t���[���Ƃ̍���
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		

	//�������Ԃɗ]�T������ꍇ�A�҂����킹���s��
	if (frameTime < FRAME_TIME)										
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	//������1�b�ȏゾ�����ꍇ
	if (diff >= 1)													
	{
		//1�b�������Ŋ���AFPS���擾
		_fps = 1 / frameTime;										

		//FPS�X�V���Ԃ��X�V
		_updatedTime = _currentTime;								
	}

	//�O�t���[���̎��Ԃ��X�V
	_beforeTime = _currentTime;										
}

/// <summary>
/// �����_�����󂯎����񏈗����s���֐�
/// </summary>
/// <param name="func">����ɏ��������������_��</param>
void
BaseScene::ParallelProcess(function<void(void)> func)
{
	//�����_������������X���b�h���쐬
	thread th(func);	
	//�X���b�h�̊Ǘ��������
	th.detach();		
}