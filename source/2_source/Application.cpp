#include "Application.h"
#include "Package/Package.h"
#include "Scene/PlayScene.h"

const unsigned int DISPLAY_WIDTH = GetSystemMetrics(SM_CXSCREEN);		//�f�B�X�v���C��
const unsigned int WINDOW_WIDTH = 1280;									//�E�B���h�E��
const unsigned int DISPLAY_HEIGHT = GetSystemMetrics(SM_CYSCREEN);		//�f�B�X�v���C����
const unsigned int WINDOW_HEIGHT = 720;									//�E�B���h�E����

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

/// <summary>
/// OS���瑗���Ă���f�[�^(���b�Z�[�W)����������֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�����ʂ���f�[�^</param>
/// <param name="msg">OS���瑗���Ă���f�[�^</param>
/// <param name="wpraram">1�ڂ̃��b�Z�[�W�̕ϐ�</param>
/// <param name="lparam" >2�ڂ̃��b�Z�[�W�̕ϐ�</param>
/// <returns></returns>
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);											//OS�ɃA�v���̏I����`����
		return 0;

	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wparam, lparam);
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_INPUT:													//�}�E�X�̓��͂�`����
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_SYSKEYDOWN:												//�L�[�{�[�h�̓��͂�`����
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);		//imgui�̃E�B���h�E�𓮂������߂ɕK�v
	return DefWindowProc(hwnd, msg, wparam, lparam);				//����̏������s��
}

/// <summary>
/// �t�H���_���̃f�[�^�����ׂč폜����֐�
/// </summary>
void
OnExit()
{
	Package package;
	//package.RemoveFileInDiretory(_T("Asset/font"));
	//package.RemoveFileInDiretory(_T("Asset/image"));
	//package.RemoveFileInDiretory(_T("Asset/model"));
	//package.RemoveFileInDiretory(_T("Asset/sound"));

	return;
}

/// <summary>
/// �Q�[���p�E�B���h�E���쐬����֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�����ʂ���f�[�^</param>
/// <param name="windowClass">�E�B���h�E�쐬�p�f�[�^���i�[����\����</param>
void 
Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass)
{
	windowClass.cbSize = sizeof(WNDCLASSEX);							//�\���̂̃������T�C�Y�w��
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;					//�R�[���o�b�N�֐��̎w��
	windowClass.lpszClassName = _T("DX12Application");					//�A�v���P�[�V�����N���X��
	windowClass.hInstance = GetModuleHandle(nullptr);					//�C���X�^���X(�A�v���P�[�V����)�n���h���̎擾

	RegisterClassEx(&windowClass);										//�A�v���P�[�V�����N���X�̎w���OS�ɓ`����

	RECT wrc = { 0,0,(LONG)WINDOW_WIDTH,(LONG)WINDOW_HEIGHT };			//�E�B���h�E�T�C�Y�̌���
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);					//�E�B���h�E�T�C�Y���v�Z

	hwnd = CreateWindow(windowClass.lpszClassName,						//�E�B���h�E�n���h���̓o�^
		_T("Program"),
		WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX ,
		(DISPLAY_WIDTH/2) - (WINDOW_WIDTH/2),
		(DISPLAY_HEIGHT / 2) - (WINDOW_HEIGHT / 2),
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		windowClass.hInstance,
		nullptr
	);

	auto hdc = GetDC(_hwnd);											//���t���b�V�����[�g�̎擾
	_rate = GetDeviceCaps(hdc, VREFRESH);

	if (_rate <= 60) { _interval = 1; }									//���[�g�ɉ����ă����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��ݒ�
	else if (_rate >= 120) { _interval = 2; }

	_deltaTime = (float)1 / (float)_rate;								//1�t���[�����̌o�ߕb�����v�Z
}

/// <summary>
/// �R���X�g���N�^
/// ���ɏ����͍s��Ȃ�
/// </summary>
Application::Application()
{

}

/// <summary>
/// �ÓI�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̎Q��</returns>
Application&
Application::Instance()
{
	static Application instance;
	return instance;
}

/// <summary>
/// �A�v���P�[�V�����������֐�
/// </summary>
/// <returns>�������������������ǂ���</returns>
bool 
Application::Init()
{
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);						//�X���b�h�������s������COM���C�u������������
	CreateGameWindow(_hwnd, _windowClass);										//�Q�[���p�E�B���h�E�쐬

	ShowCursor(true);															//�}�E�X�J�[�\����\������

	atexit(OnExit);																//�I���n���h���[�Ɋ֐���o�^

	_package.reset(new Package());												//Package�C���X�^���X��������

	//_package->CreateDirectoryAndFile(_T("Asset/font.pak"));					//.pak�t�@�C������f�[�^��ǂݍ���
	//_package->CreateDirectoryAndFile(_T("Asset/image.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/model.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/sound.pak"));

	//_package->CreatePackage(_T("Asset/font"), _T("font.pak"));				//.pak�t�@�C���쐬����
	//_package->CreatePackage(_T("Asset/image"), _T("image.pak"));
	//_package->CreatePackage(_T("Asset/model"), _T("model.pak"));
	//_package->CreatePackage(_T("Asset/sound"), _T("sound.pak"));

	_dx12.reset(new Dx12Wrapper(_hwnd,_deltaTime));								//Dx12Wrapper�C���X�^���X��������
	
	_input.reset(new InputManager());											//InputManager�C���X�^���X��������

	_sound.reset(new SoundManager());											//SoundManager�C���X�^���X��������

	_play.reset(new PlayScene(*_dx12, *_input, *_sound));						//�Q�[���V�[���̏�����

	ShowWindow(_hwnd, SW_SHOW);													//�E�B���h�E�n���h���ɑΉ�����E�B���h�E��\��

	_play->SceneStart();

	return true;
}

/// <summary>
/// �Q�[����ʂ�`�悷��֐�
/// </summary>
void
Application::Run()
{
	//�Q�[�����[�v
	while (true)
	{
 		//�X���b�h�֌W�̏���
		if (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE))		//���b�Z�[�W�𒲂ׂ�
		{
			TranslateMessage(&_msg);							//���b�Z�[�W��|��
			DispatchMessage(&_msg);								//�E�B���h�E�v���V�[�W���փ��b�Z�[�W�𑗂�

			if (_msg.message == WM_QUIT)						//�A�v�����I�������烋�[�v���I��������
			{
				break;
			}
		}
		else
		{
			_play->Update();									//�e�V�[���̍X�V����
		}
	}
}

/// <summary>
/// �A�v���I�����A��n�����s���֐�
/// </summary>
void 
Application::Terminate()
{
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);	//�\���̂̐ݒ������
}

/// <summary>
/// �E�B���h�E�T�C�Y��Ԃ��֐�
/// </summary>
/// <returns>�E�B���h�E�T�C�Y�������\����</returns>
SIZE
Application::GetWindowSize()const
{
	SIZE ret;					//�\���̂�錾���A���ƍ�����ݒ�
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;

	return ret;					//�\���̂�Ԃ�
}

/// <summary>
/// �Q�[�����I��������
/// </summary>
void
Application::ExitApp()
{
	SendMessage(_hwnd,WM_DESTROY,0,0);	//�I���p���b�Z�[�W�𑗂�
}

/// <summary>
/// �����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��Ԃ��֐�
/// </summary>
/// <returns>�t���b�v�Ԋu</returns>
int
Application::GetInterval()const
{
	return _interval;
}

/// <summary>
/// 1�b������̃t���[�����[�g��Ԃ��֐�
/// </summary>
/// <returns>�t���[�����[�g</returns>
int
Application::GetRate()const
{
	return _rate;
}

/// <summary>
/// �f�X�g���N�^
/// �|�C���^���J������
/// </summary>
Application::~Application()
{	

}