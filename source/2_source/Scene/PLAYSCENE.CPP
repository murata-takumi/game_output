#include "Functions.h"
#include "Scene/PlayScene.h"
#include "Constant.h"

const XMVECTOR Y_VECTOR = XMVectorSet(0.0f,90.0f,0.0f,0.0f);	//Y���x�N�g��
const wchar_t* ACTOR_PATH = L"Asset/model/Unitychan.fbx";		//���f���f�[�^���i�[����Ă���t�@�C���p�X
const wchar_t* GROUND_PATH = L"Asset/model/G.fbx";				//�n�ʃf�[�^���i�[����Ă���t�@�C���p�X
const float FRAME_TIME = 1.0f / 60.0f;							//�X�V�񐔂�60FPS�ɌŒ肷��ۂ�1�t���[���̕b��

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="input">InputManager�C���X�^���X</param>
/// <param name="sound">SoundManager�C���X�^���X</param>
PlayScene::PlayScene(
	Dx12Wrapper& dx12,
	InputManager& input,
	SoundManager& sound
)
	:_dx12(dx12), _input(input), _sound(sound), _app(Application::Instance()), _fps(0.0f)
{
	_interval = _app.GetInterval();															//�t���b�v�Ԋu���擾

	_renderer.reset(new Renderer(_dx12));
	_pera.reset(new PeraRenderer(_dx12));
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
PlayScene::~PlayScene()
{
	
}

/// <summary>
/// �X�V����
/// </summary>
void
PlayScene::Update()
{
	InputUpdate();													//���͂��X�V

	if (!_isInLoad)													//����\�ȏꍇ���͂𔽉f����
	{
		auto cols0 = _actorAndObjs[0]->Collider()->BoundValues();
		auto cols1 = _actorAndObjs[1]->Collider()->BoundValues();

		_actor->SetOnGround(CheckIntersect(cols0, cols1));

		//WASD�L�[�̓��͂ɉ����Ď��_���ړ�������
		//�����_�𒆐S�Ɏ��_����]
		if (_input.KeyState().A)
		{
			_dx12.RotateCoordinates(Degree::Azimth, 45);
		}
		if (_input.KeyState().D)
		{
			_dx12.RotateCoordinates(Degree::Azimth, -45);
		}
		if (_input.KeyState().W)
		{
			_dx12.RotateCoordinates(Degree::Elevation, 30);
		}
		if (_input.KeyState().S)
		{
			_dx12.RotateCoordinates(Degree::Elevation, -30);
		}

		//Q�E�L�[�����������͊g��E�k��
		//���V�t�g�L�[�������Ă���ꍇ�̓J�������㉺�ɕ��s�ړ�������
		if (_input.KeyState().Q)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(-90);
			}
		}
		if (_input.KeyState().E)
		{
			if (_input.KeyState().LeftShift)
			{
				_dx12.TranslateCoordinates(Y_VECTOR * -1 * 0.1f);
			}
			else
			{
				_dx12.ScalingCoordinates(90);
			}
		}
		//�A�N�^�[���ړ��\���ǂ���
		if (_dx12.ImGui()->CanActorTrans())
		{
			if ((_input.KeyState().Left ||
				_input.KeyState().Right ||
				_input.KeyState().Up    ||
				_input.KeyState().Down) &&
				!_actor->IsAnimationEqual(RUN00_F)
				)
			{
				_actor->SetAnimationNode(RUN00_F);
			}

			if (_input.KeyState().Left)
			{
				_direction += _dx12.GetRightVector();				//�J�����ɑ΂��������Ƀx�N�g�����Z
			}
			if (_input.KeyState().Right)
			{
				_direction += _dx12.GetRightVector() * -1;			//�J�����ɑ΂��E�����Ƀx�N�g�����Z
			}
			if (_input.KeyState().Up)
			{
				_direction += _dx12.GetXZVecEyeToTarget();			//�J�����ɑ΂��������Ƀx�N�g�����Z
			}
			if (_input.KeyState().Down)
			{
				_direction += _dx12.GetXZVecEyeToTarget() * -1;		//�J�����ɑ΂���O�����Ƀx�N�g�����Z
			}

			if (XMVector3Length(_direction).m128_f32[0] > 0.0f)
			{
				_direction = XMVector3Normalize(_direction);		//�ړ��x�N�g���𐳋K��
				_actor->Translate(_direction);						//�A�N�^�[���ړ�
				_dx12.TranslateCoordinates(_direction);				//�J�������ړ�

				_direction = XMVectorZero();						//�ړ��x�N�g����������
			}

			if ((!_input.KeyState().Left	&&
				!_input.KeyState().Right	&&
				!_input.KeyState().Up		&&
				!_input.KeyState().Down)    &&
				!_actor->IsAnimationEqual(WAIT00)
				)
			{
				_actor->SetAnimationNode(WAIT00);
			}

			if (_input.KeyTracker().IsKeyPressed(Keyboard::Space) &&
				!_actor->IsAnimationEqual(JUMP00))
			{
				_actor->SetAnimationNode(JUMP00);
			}
		}
		else
		{

		}

		//�L�[�p�b�h�̓��͂ɉ����ăJ�����𐳖ʁE���ʁE��ʂɔz�u
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad1))
		{
			_dx12.ResetCoordinates(0, 0);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad3))
		{
			_dx12.ResetCoordinates(XM_PIDIV2, 0);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad5))
		{
			auto isPers = _dx12.Perspective();
			_dx12.SetPersOrOrtho(!isPers);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad7))
		{
			_dx12.ResetCoordinates(0, XM_PIDIV2);
		}
		if (_input.KeyTracker().IsKeyPressed(Keyboard::NumPad9))
		{
			_dx12.ResetCoordinates(0, -1 * XM_PIDIV2);
		}
	}

	//�`�揈��
	DrawUpdate();
}

/// <summary>
/// �V�[���J�n���Ɏ��s����֐�
/// </summary>
void
PlayScene::SceneStart()
{
	//�J�����ʒu�����Z�b�g
	_dx12.ResetCoordinates(0,0);								

	//�v���C���[�̈ړ��x�N�g����������
	_direction = XMVectorZero();										

	//���f���ǂݍ��ݏ���
	auto f = [&]()
	{
		//���[�h�����Ǝ���
		_isInLoad = true;												

		//���񏈗��������X���b�h�̃x�N�g��
		vector<thread> ths;												
		shared_ptr<FBXObject> ground;

		//�X���b�h�ɃA�N�^�[�̍쐬������ǉ�
		ths.emplace_back
		(	
			[&](){
				lock_guard lock(_mtx);

				_actor = make_shared<FBXActor>(_dx12, ACTOR_PATH);		
			}
		);
		//�t�B�[���h�̍쐬������ǉ�
		ths.emplace_back
		(
			[&]() {
				lock_guard lock(_mtx);

				ground = make_shared<FBXObject>(_dx12, GROUND_PATH);
			}
		);

		//�X���b�h���S�Ċ�������܂ł����őҋ@
		for (auto& th : ths)											
		{
			th.join();
		}

		//�`��\��̃I�u�W�F�N�g�ɃA�N�^�[�A�t�B�[���h��ǉ�
		_actorAndObjs.emplace_back(ground);
		_actorAndObjs.emplace_back(_actor);

		//�A�N�^�[��ImGuiManager���ɓn��
		_dx12.ImGui()->SetActor(_actorAndObjs);							
		//�A�N�^�[��SpriteManager���ɓn��
		_dx12.Sprite()->SetActors(_actorAndObjs);						

		//���[�h����
		_isInLoad = false;		

		//�t�F�[�h�C����������񏈗�
		auto startFunc = [&]()
		{
			_dx12.Fade(1.0f, 0.0f);										
		};
		ParallelProcess(startFunc);

		//�G�t�F�N�g���s
		//_dx12.Effect()->CallEffect();									
	};
	//��L�̓ǂݍ��ݏ�����ʏ�̕`�揈���ƕ��s���čs��
	ParallelProcess(f);													
}

/// <summary>
/// �V�[���J�ڎ��Ɏ��s����֐�
/// </summary>
void
PlayScene::SceneEnd()
{
	//�I�u�W�F�N�g���܂Ƃ߂č폜
	_actorAndObjs.clear();
	//�A�N�^�[��������
	_actor = nullptr;					

	//ImGui�ŕ\������Ă���A�j���[�V��������������
	_dx12.ImGui()->ResetAnimStr();		
}

/// <summary>
/// ���͂��X�V����֐�
/// </summary>
void
PlayScene::InputUpdate()
{
	_input.UpdateInput();
}

/// <summary>
/// ���݂̃t���[�����[�g���X�V����֐�
/// </summary>
void
PlayScene::UpdateFPS()
{
	//���݃t���[���̎��Ԃ��擾
	QueryPerformanceCounter(&_currentTime);							

	//�O��FPS�X�V���Ƃ̎��Ԃ̍���
	auto diff = GetLIntDiff(_currentTime, _updatedTime);			
	//�O�t���[���Ƃ̎��Ԃ̍���
	auto frameTime = GetLIntDiff(_currentTime, _beforeTime);		

	//�������Ԃɗ]�T������ꍇ�A������҂����킹��
	if (frameTime < FRAME_TIME)										
	{
		DWORD sleepTime =
			static_cast<DWORD>((FRAME_TIME - frameTime) * 1000);
		timeBeginPeriod(1);

		Sleep(sleepTime);

		timeEndPeriod(1);
	}

	//�O��FPS�X�V����1�b�ȏソ������
	if (diff >= 1)													
	{
		//1�b��O�t���[���Ƃ̎��Ԃ̍����Ŋ���AFPS���擾
		_fps = 1 / frameTime;
		_updatedTime = _currentTime;
	}

	//�O�t���[���̎��Ԃ��X�V
	_beforeTime = _currentTime;										
}

/// <summary>
/// �e�`��̍X�V���܂Ƃ߂čs���֐�
/// </summary>
void
PlayScene::DrawUpdate()
{
	//FPS���X�V
	UpdateFPS();			

	//�y���|���S���`�揈��
	PeraDraw();				

	//�Q�[����ʕ`�揈��
	GameDraw();				
}

/// <summary>
/// �y���|���S���̕`�揈�����܂Ƃ߂��֐�
/// </summary>
void
PlayScene::PeraDraw()
{
	//�y���|���S���`�揀��
	_pera->BeginPeraDraw();						

	//�y���|���S���p�p�C�v���C�����Z�b�g
	_dx12.SetPipelines(							
		_renderer->GetRootSignature(),
		_renderer->GetPipelineState(),
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	);

	//�r���[�E�v���W�F�N�V�����s���K�p
	_dx12.SetScene();							

	//���f���`�揈��
	ModelDraw();								

	//�y���|���S���`���n��
	_pera->EndPeraDraw();						
}

/// <summary>
/// �Q�[����ʕ`�揈��
/// </summary>
void
PlayScene::GameDraw()
{
	//�Q�[����ʕ`�揀��
	_dx12.BeginGameDraw();															

	//�ȉ��Q�[����ʗp�p�C�v���C�����Z�b�g
	_pera->SetPeraPipelines();														

	_dx12.CommandList()->IASetVertexBuffers(0, 1, _pera->PeraVBView());				//�y���|���S���pVBV���Z�b�g
	_dx12.CommandList()->DrawInstanced(4, 1, 0, 0);									//�y���|���S�����\�����钸�_��`��

	if (_dx12.ImGui()->IsWriteGrid() && !_isInLoad)_dx12.Sprite()->GridDraw();		//�O���b�h��`��

	EffectAndUIDraw();																//�����ɃG�t�F�N�g��̕`�揈��

	_dx12.EndGameDraw();															//�Q�[����ʕ`���n��

	_dx12.Swapchain()->Present(_interval, 0);										//�X���b�v�`�F�[���̃t���b�v����

	_dx12.Sprite()->Commit();														//�O���t�B�b�N�X��������ݒ�
}

/// <summary>
/// ��̓����蔻�肪�Փ˂��Ă��邩�m�F����֐�
/// </summary>
/// <param name="col0">�����蔻�肻��1</param>
/// <param name="col1">�����蔻�肻��2</param>
/// <returns>�Փ˂��Ă��邩�ǂ���</returns>
bool 
PlayScene::CheckIntersect(map<string, float> col0, map<string, float> col1)
{
	if ((col0[Constant::BOX_COL_X_MIN] - col1[Constant::BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_X_MIN] - col0[Constant::BOX_COL_X_MAX]) > FLT_EPSILON)return false;
	if ((col0[Constant::BOX_COL_Y_MAX] - col1[Constant::BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_Y_MAX] - col0[Constant::BOX_COL_Y_MIN]) > FLT_EPSILON)return false;
	if ((col0[Constant::BOX_COL_Z_MAX] - col1[Constant::BOX_COL_Z_MIN]) > FLT_EPSILON)return false;
	if ((col1[Constant::BOX_COL_Z_MAX] - col0[Constant::BOX_COL_Z_MIN]) > FLT_EPSILON)return false;

	return true;
}

/// <summary>
/// ���f���`����s���֐�
/// </summary>
void
PlayScene::ModelDraw()
{
	if (!_isInLoad)									//�v���C���[���ǂݍ��݊������Ă���ꍇ�`�揈��
	{
		for (const auto& item : _actorAndObjs)		//FBXBase�N���X����ՂƂ���I�u�W�F�N�g�S�ʂɑ΂��`��E�X�V�������s��
		{
			if (item != nullptr)
			{
				item->Draw();
				item->Update();
			}
		}
	}
}

/// <summary>
/// �G�t�F�N�g��̕`����s���֐�
/// </summary>
void
PlayScene::EffectAndUIDraw()
{
	if (_isInLoad)						//���[�h���̏ꍇ
	{
		_dx12.Sprite()->LoadingDraw();	//���[�h��ʂ�`��
	}
	else								//����\�ȏꍇ
	{
		_dx12.ImGui()->SetFPS(_fps);	//Scene����ImGui�Ƀf�[�^��n��
		_dx12.ImGui()->ImGuiDraw();		//ImGui�̃E�B���h�E��`��
	}
}