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
		//OS�ɃA�v���̏I����`����
		PostQuitMessage(0);											
		return 0;

	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wparam, lparam);
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_INPUT:													
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
		//�}�E�X�̓��͂�`����
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_SYSKEYDOWN:												
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		//�L�[�{�[�h�̓��͂�`����
		Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}

	//imgui�̃E�B���h�E�𓮂������߂ɕK�v
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);		

	//����̏������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);				
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
	//�E�B���h�E�쐬�p�f�[�^�̐ݒ�
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = _T("DX12Application");
	windowClass.hInstance = GetModuleHandle(nullptr);

	//�A�v���P�[�V�����N���X�̎w���OS�ɓ`����
	RegisterClassEx(&windowClass);										

	//�E�B���h�E�T�C�Y�̌���
	RECT wrc = { 0,0,(LONG)WINDOW_WIDTH,(LONG)WINDOW_HEIGHT };			
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�n���h���̓o�^
	hwnd = CreateWindow(windowClass.lpszClassName,						
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

	//���t���b�V�����[�g�̎擾
	auto hdc = GetDC(_hwnd);											
	_rate = GetDeviceCaps(hdc, VREFRESH);

	//���[�g�ɉ����ă����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��ݒ�
	if (_rate <= 60) { _interval = 1; }									
	else if (_rate >= 120) { _interval = 2; }

	//1�t���[�����̌o�ߕb�����v�Z
	_deltaTime = (float)1 / (float)_rate;								
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
	//�X���b�h�������s������COM���C�u������������
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);		

	//�Q�[���p�E�B���h�E�쐬
	CreateGameWindow(_hwnd, _windowClass);										

	//�}�E�X�J�[�\����\������
	ShowCursor(true);															

	//�I���n���h���[�Ɋ֐���o�^
	atexit(OnExit);																

	//.pak�t�@�C������f�[�^��ǂݍ��݁A�f�ރt�@�C���쐬
	//_package.reset(new Package());
	//_package->CreateDirectoryAndFile(_T("Asset/font.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/image.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/model.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/sound.pak"));
	//_package->CreatePackage(_T("Asset/font"), _T("font.pak"));				
	//_package->CreatePackage(_T("Asset/image"), _T("image.pak"));
	//_package->CreatePackage(_T("Asset/model"), _T("model.pak"));
	//_package->CreatePackage(_T("Asset/sound"), _T("sound.pak"));

	//Dx12Wrapper�C���X�^���X��������
	Dx12Wrapper::Instance().Init(_hwnd, _deltaTime);
	
	//���͊֘A��������
	InputManager::Instance().Init();

	//BGM,SE�֘A��������
	SoundManager::Instance().Init();

	//�X�v���C�g�����������
	SpriteManager::Instance().Init(WINDOW_WIDTH,WINDOW_HEIGHT);

	//ImGui�����������
	ImGuiManager::Instance().Init(_hwnd);

	//�p�C�v���C����������
	Renderer::Instance().Init();
	PeraRenderer::Instance().Init();

	//�E�B���h�E�n���h���ɑΉ�����E�B���h�E��\��
	ShowWindow(_hwnd, SW_SHOW);													

	//�Q�[���V�[���̏�����
	_play.reset(new PlayScene());
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
		if (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE))		
		{
			//���b�Z�[�W�𒲂ׁA�v���V�[�W���փ��b�Z�[�W�𑗂�
			TranslateMessage(&_msg);
			DispatchMessage(&_msg);

			//�A�v�����I�������烋�[�v���I��������
			if (_msg.message == WM_QUIT)						
			{
				break;
			}
		}
		//�V�[���̍X�V����
		else
		{			
			_play->Update();									
		}
	}
}

/// <summary>
/// �A�v���I�����A��n�����s���֐�
/// </summary>
void 
Application::Terminate()
{
	//�\���̂̐ݒ������
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);	
}

/// <summary>
/// �E�B���h�E�T�C�Y��Ԃ��֐�
/// </summary>
/// <returns>�E�B���h�E�T�C�Y�������\����</returns>
SIZE
Application::GetWindowSize()const
{
	//�\���̂�錾���A���ƍ�����ݒ�
	SIZE ret;					
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;

	return ret;
}

/// <summary>
/// �Q�[�����I��������
/// </summary>
void
Application::ExitApp()
{
	//�I���p���b�Z�[�W�𑗂�
	SendMessage(_hwnd,WM_DESTROY,0,0);	
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