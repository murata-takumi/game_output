#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

//�t�H���g���i�[���Ă���p�X
const char* FONT_PATH = "Asset/font/BIZUDPGothic-Bold.ttf";

/// <summary>
/// �R���X�g���N�^
/// ImGui�֘A�̏��������s��
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
ImGuiManager::ImGuiManager():
	_windowWidth(static_cast<float>(Application::Instance().GetWindowSize().cx)),
	_windowHeight(static_cast<float>(Application::Instance().GetWindowSize().cy)),
	_size(18.0f),_speed(1.0f),_animSliderValue(0.0f), _fps(0.0f)
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
ImGuiManager::~ImGuiManager()
{

}

/// <summary>
/// �������֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
void
ImGuiManager::Init(HWND hwnd)
{
	_fpsFlag |= ImGuiWindowFlags_NoMove;								//FPS��\������E�B���h�E�������Ȃ��悤�ݒ�
	_fpsFlag |= ImGuiWindowFlags_NoTitleBar;							//�^�C�g���o�[��\�����Ȃ�
	_fpsFlag |= ImGuiWindowFlags_NoResize;								//�E�B���h�E�T�C�Y��ς��Ȃ�
	_fpsFlag |= ImGuiWindowFlags_NoScrollbar;							//�X�N���[���o�[��\�����Ȃ�

	_animFlag |= ImGuiWindowFlags_NoMove;								//�A�j���[�V�����\���E�B���h�E�����l�ɐݒ�
	_animFlag |= ImGuiWindowFlags_NoResize;								//�E�B���h�E�T�C�Y��ς��Ȃ�

	if (ImGui::CreateContext() == nullptr)								//imgui�̏�����
	{
		assert(0);
		return;
	}
	bool bInResult = ImGui_ImplWin32_Init(hwnd);
	if (!bInResult)
	{
		assert(0);
		return;
	}

	Dx12Wrapper::Instance().CreateDescriptorHeap(_heapForImgui,							//ImGui�p�q�[�v���쐬
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 0, 1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);


	bInResult = ImGui_ImplDX12_Init(
		Dx12Wrapper::Instance().Device(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		_heapForImgui.Get(),
		_heapForImgui->GetCPUDescriptorHandleForHeapStart(),
		_heapForImgui->GetGPUDescriptorHandleForHeapStart());
	if (!bInResult)
	{
		assert(0);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();										//�t�H���g��ݒ�
	io.Fonts->AddFontFromFileTTF(
		FONT_PATH,
		_size, NULL, io.Fonts->GetGlyphRangesJapanese());
}

/// <summary>
/// �V���O���g����Ԃ��֐�
/// </summary>
/// <returns>�V���O���g��</returns>
ImGuiManager&
ImGuiManager::Instance()
{
	static ImGuiManager instance;
	return instance;
}

bool
ImGuiManager::CanActorTrans()const
{
	return _canActorControll;
}

bool
ImGuiManager::IsWriteGrid()const
{
	return _isWriteGrid;
}

/// <summary>
/// �e��E�B���h�E�̕`�����������֐�
/// </summary>
void
ImGuiManager::ImGuiDraw()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//FPS�E�B���h�E�̕\������
	{
		ImGui::Begin("FPS", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(DIFF,DIFF));												//�E�B���h�E�ʒu������
		ImGui::SetWindowSize(ImVec2(_size * 11, _size * 3));								//�E�B���h�E�T�C�Y��ݒ�

		if (_fps <= 30.0f)ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));	//FPS��30�ȉ��̏ꍇ�����̐F��Ԃ�����
		ImGui::Text("FPS:%.1f", _fps);														//FPS��\��
		if (_fps <= 30.0f)ImGui::PopStyleColor();

		ImGui::Text(Dx12Wrapper::Instance().Perspective() ? "Perspective" : "OrthoGraphic");					//�������e�����s���e���\��

		ImGui::End();
	}

	//�A�N�^�[���ړ�����ۂ̃E�B���h�E
	{
		ImGui::Begin("Translation", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(_windowWidth - (DIFF + _size * 18), DIFF));
		ImGui::SetWindowSize(ImVec2(_size * 18,_size * 36));

		ImGui::Checkbox("WriteGrid", &_isWriteGrid);										//�O���b�h��`�悷�邩���߂�

		if (ImGui::Checkbox("CanActorControll", &_canActorControll))						//�A�N�^�[���ړ��\���ǂ������߂�
		{
			if (_canActorControll)															//�`�F�b�N�{�b�N�X��true�ɂ�����
			{
				_actor->StartControll();

				_canCallAnim = false;
			}
			else																			//�`�F�b�N�{�b�N�X��false�ɂ�����
			{
				_actor->EndControll();														//�A�N�^�[�̈ړ��E��]��������
				Dx12Wrapper::Instance().ResetCoordinates(0.0f,0.0f);											//�J������������
			}
		}

		ImGui::Checkbox("IsOnGround", &_actor->_isOnGround);

		ImGui::End();
	}

	//�A�j���[�V����������ׂ��E�B���h�E�̕\������
	{
		ImGui::Begin("Animation",0,_animFlag);

		ImGui::SetWindowPos(ImVec2(DIFF, _windowHeight - (ANIMATION_HEIGHT + DIFF)));		//�E�B���h�E���W��ݒ�
		ImGui::SetWindowSize(ImVec2(_windowWidth - (DIFF * 2),ANIMATION_HEIGHT));			//�E�B���h�E�T�C�Y��ݒ�
		ImGui::BeginDisabled(_canActorControll);											//�A�N�^�[���ړ��\�ȂƂ��͂��̃E�B���h�E��񊈐���

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
		if (ImGui::BeginCombo("##", _currentImGuiAnim.c_str()))
		{
			for (int i = 0; i < _animStr.size(); ++i)
			{
				_comboSelect = (_currentImGuiAnim.c_str() == _animStr[i].c_str());
				if (ImGui::Selectable(_animStr[i].c_str(), _comboSelect))
				{
					_currentImGuiAnim = _animStr[i].c_str();

					_maxDur = _actor->GetAnimDuration(_currentImGuiAnim) - _actor->GetSecondFrame();
					_tick = _actor->GetAnimTickPerSpeed(_currentImGuiAnim);
					_animSliderValue = 0.0f;

					_actor->InitPose();
					_actor->SetAnimationTime(_animSliderValue);										//�X���C�_�[�̒l���A�N�^�[�ɓn��

					_canCallAnim = false;
				}

				if (_comboSelect)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if(ImGui::Button("Resume"))
		{
			QueryPerformanceCounter(&_beforeFrameTime);										//�A�j���[�V�����J�n���̃t���[���̎��Ԃ��擾

			_animSliderValue = 0.0f;

			_actor->SetAnimStr(_currentImGuiAnim);											//���s����A�j���[�V�������w��
			_actor->SetAnimationTime(_animSliderValue);										//���s����A�j���[�V�������w��

			_canCallAnim = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			_animSliderValue = 0.0f;

			_actor->InitPose();																//�A�N�^�[�̃|�[�Y�����Z�b�g����
			_actor->SetAnimationTime(_animSliderValue);										//�X���C�_�[�̒l���A�N�^�[�ɓn��

			_canCallAnim = false;
		}

		ImGui::SliderFloat("###", &_speed, 0.1f, 2.0f);										//�A�j���[�V�����̍Đ����x
		
		ImGui::SameLine();

		if (ImGui::Button("1.0"))															//�Đ����x��1�ɒ���
		{
			_speed = 1.0f;
		}
		ImGui::PopItemWidth();

		_animSliderValue = clamp(_animSliderValue,0.0f, _maxDur);
		if (ImGui::SliderFloat("Animation", &_animSliderValue, 0.0f, _maxDur))
		{
			if (_canCallAnim)_canCallAnim = false;									//�A�j���[�V�������s����������~�߂�
			_actor->SetAnimationTime(_animSliderValue);								//�X���C�_�[�̒l���A�N�^�[�ɓn��
		}

		ImGui::EndDisabled();
		ImGui::End();
	}

	Update();																		//�`��ȊO�̏���

	ImGui::Render();

	ID3D12DescriptorHeap* heap[] = { _heapForImgui.Get()};							//ImGui�p�q�[�v���Z�b�g
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Dx12Wrapper::Instance().CommandList());
}

/// <summary>
/// �V�[������󂯎�����f�[�^�𔽉f����֐�
/// </summary>
/// <param name="data"FPS</param>
void
ImGuiManager::SetFPS(float fps)
{
	_fps = fps;
}

/// <summary>
/// �A�N�^�[�ƒn�ʂ̍��W���Ƃ̐^���l�̓����蔻����󂯎��֐�
/// </summary>
/// <param name="intersects">�^���l�x�N�g��</param>
void
ImGuiManager::SetActorColBool(bool intersects[])
{
	for(int i = 0; i < 6; i++)
	{
		_intersects[i] = intersects[i];
	}
}

/// <summary>
/// �A�N�^�[���󂯎��֐�
/// </summary>
/// <param name="actor">�A�N�^�[�̃x�N�g��</param>
void
ImGuiManager::SetActor(vector<shared_ptr<FBXBase>> actorAndObjects)
{
	_ground = actorAndObjects[1];

	//�x�N�g���̒�����A�N�^�[��T���Ď擾
	for (const auto& object : actorAndObjects)						
	{
		if (_actor = dynamic_pointer_cast<FBXActor>(object))
		{
			break;
		}
	}

	//�A�N�^�[�̃A�j���[�V�������������瑤�Ɋi�[����
	for (auto str : _actor->GetAnimstr())							
	{
		_animStr.push_back(str);
	}

	//�����A�j���[�V������ݒ�
	_currentImGuiAnim = _animStr[0];								

	//�A�j���[�V�����̑����Ԃ��擾
	_maxDur = _actor->GetAnimDuration(_currentImGuiAnim)			
				- _actor->GetSecondFrame();
	//�A�j���[�V�����̏����񐔂��擾
	_tick = _actor->GetAnimTickPerSpeed(_currentImGuiAnim);			
	//2�ڂ̃t���[�����擾
	_animSliderValue = 0.0f;										
}

/// <summary>
/// �A�j���[�V�������̔z�������������֐�
/// </summary>
void
ImGuiManager::ResetAnimStr()
{
	_animStr.clear();
}

/// <summary>
/// �`��ȊO�̏��������s����֐�
/// </summary>
void
ImGuiManager::Update()
{
	if (_canCallAnim)
	{
		QueryPerformanceCounter(&_crrntFrameTime);								//���t���[���̎��Ԃ��擾

		auto diff = GetLIntDiff(_crrntFrameTime, _beforeFrameTime);				//���t���[���ƑO�t���[���̎��Ԃ̍�����float�ɕϊ�

		_animSliderValue +=														//���������Z
			static_cast<float>(diff) * _speed * _tick;

		_beforeFrameTime = _crrntFrameTime;										//�O�t���[���̎��Ԃ��X�V

		_actor->SetAnimationTime(_animSliderValue);								//�X���C�_�[�̒l���A�N�^�[�ɓn��
	}
}