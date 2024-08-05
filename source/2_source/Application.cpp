#include "Application.h"
#include "Package/Package.h"
#include "Scene/PlayScene.h"

const unsigned int DISPLAY_WIDTH = GetSystemMetrics(SM_CXSCREEN);		//ディスプレイ幅
const unsigned int WINDOW_WIDTH = 1280;									//ウィンドウ幅
const unsigned int DISPLAY_HEIGHT = GetSystemMetrics(SM_CYSCREEN);		//ディスプレイ高さ
const unsigned int WINDOW_HEIGHT = 720;									//ウィンドウ高さ

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND,UINT,WPARAM,LPARAM);

/// <summary>
/// OSから送られてくるデータ(メッセージ)を処理する関数
/// </summary>
/// <param name="hwnd">ウィンドウを識別するデータ</param>
/// <param name="msg">OSから送られてくるデータ</param>
/// <param name="wpraram">1つ目のメッセージの変数</param>
/// <param name="lparam" >2つ目のメッセージの変数</param>
/// <returns></returns>
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		//OSにアプリの終了を伝える
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
		//マウスの入力を伝える
		Mouse::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_SYSKEYDOWN:												
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		//キーボードの入力を伝える
		Keyboard::ProcessMessage(msg, wparam, lparam);
		break;

	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	}

	//imguiのウィンドウを動かすために必要
	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);		

	//既定の処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);				
}

/// <summary>
/// フォルダ内のデータをすべて削除する関数
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
/// ゲーム用ウィンドウを作成する関数
/// </summary>
/// <param name="hwnd">ウィンドウを識別するデータ</param>
/// <param name="windowClass">ウィンドウ作成用データを格納する構造体</param>
void 
Application::CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass)
{
	//ウィンドウ作成用データの設定
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = (WNDPROC)WindowProcedure;
	windowClass.lpszClassName = _T("DX12Application");
	windowClass.hInstance = GetModuleHandle(nullptr);

	//アプリケーションクラスの指定をOSに伝える
	RegisterClassEx(&windowClass);										

	//ウィンドウサイズの決定
	RECT wrc = { 0,0,(LONG)WINDOW_WIDTH,(LONG)WINDOW_HEIGHT };			
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウハンドルの登録
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

	//リフレッシュレートの取得
	auto hdc = GetDC(_hwnd);											
	_rate = GetDeviceCaps(hdc, VREFRESH);

	//レートに応じてレンダーターゲットのフリップ間隔を設定
	if (_rate <= 60) { _interval = 1; }									
	else if (_rate >= 120) { _interval = 2; }

	//1フレーム毎の経過秒数を計算
	_deltaTime = (float)1 / (float)_rate;								
}

/// <summary>
/// コンストラクタ
/// 特に処理は行わない
/// </summary>
Application::Application()
{

}

/// <summary>
/// 静的インスタンスを返す関数
/// </summary>
/// <returns>インスタンスの参照</returns>
Application&
Application::Instance()
{
	static Application instance;
	return instance;
}

/// <summary>
/// アプリケーション初期化関数
/// </summary>
/// <returns>初期化が成功したかどうか</returns>
bool 
Application::Init()
{	
	//スレッド処理を行うためCOMライブラリを初期化
	auto result = CoInitializeEx(0, COINIT_MULTITHREADED);		

	//ゲーム用ウィンドウ作成
	CreateGameWindow(_hwnd, _windowClass);										

	//マウスカーソルを表示する
	ShowCursor(true);															

	//終了ハンドラーに関数を登録
	atexit(OnExit);																

	//.pakファイルからデータを読み込み、素材ファイル作成
	//_package.reset(new Package());
	//_package->CreateDirectoryAndFile(_T("Asset/font.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/image.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/model.pak"));
	//_package->CreateDirectoryAndFile(_T("Asset/sound.pak"));
	//_package->CreatePackage(_T("Asset/font"), _T("font.pak"));				
	//_package->CreatePackage(_T("Asset/image"), _T("image.pak"));
	//_package->CreatePackage(_T("Asset/model"), _T("model.pak"));
	//_package->CreatePackage(_T("Asset/sound"), _T("sound.pak"));

	//Dx12Wrapperインスタンスを初期化
	Dx12Wrapper::Instance().Init(_hwnd, _deltaTime);
	
	//入力関連を初期化
	InputManager::Instance().Init();

	//BGM,SE関連を初期化
	SoundManager::Instance().Init();

	//スプライト周りを初期化
	SpriteManager::Instance().Init(WINDOW_WIDTH,WINDOW_HEIGHT);

	//ImGui周りを初期化
	ImGuiManager::Instance().Init(_hwnd);

	//パイプラインを初期化
	Renderer::Instance().Init();
	PeraRenderer::Instance().Init();

	//ウィンドウハンドルに対応するウィンドウを表示
	ShowWindow(_hwnd, SW_SHOW);													

	//ゲームシーンの初期化
	_play.reset(new PlayScene());
	_play->SceneStart();

	return true;
}

/// <summary>
/// ゲーム画面を描画する関数
/// </summary>
void
Application::Run()
{
	//ゲームループ
	while (true)
	{
 		//スレッド関係の処理
		if (PeekMessage(&_msg, nullptr, 0, 0, PM_REMOVE))		
		{
			//メッセージを調べ、プロシージャへメッセージを送る
			TranslateMessage(&_msg);
			DispatchMessage(&_msg);

			//アプリが終了したらループを終了させる
			if (_msg.message == WM_QUIT)						
			{
				break;
			}
		}
		//シーンの更新処理
		else
		{			
			_play->Update();									
		}
	}
}

/// <summary>
/// アプリ終了時、後始末を行う関数
/// </summary>
void 
Application::Terminate()
{
	//構造体の設定を解除
	UnregisterClass(_windowClass.lpszClassName, _windowClass.hInstance);	
}

/// <summary>
/// ウィンドウサイズを返す関数
/// </summary>
/// <returns>ウィンドウサイズを示す構造体</returns>
SIZE
Application::GetWindowSize()const
{
	//構造体を宣言し、幅と高さを設定
	SIZE ret;					
	ret.cx = WINDOW_WIDTH;
	ret.cy = WINDOW_HEIGHT;

	return ret;
}

/// <summary>
/// ゲームを終了させる
/// </summary>
void
Application::ExitApp()
{
	//終了用メッセージを送る
	SendMessage(_hwnd,WM_DESTROY,0,0);	
}

/// <summary>
/// レンダーターゲットのフリップ間隔を返す関数
/// </summary>
/// <returns>フリップ間隔</returns>
int
Application::GetInterval()const
{
	return _interval;
}

/// <summary>
/// 1秒あたりのフレームレートを返す関数
/// </summary>
/// <returns>フレームレート</returns>
int
Application::GetRate()const
{
	return _rate;
}

/// <summary>
/// デストラクタ
/// ポインタを開放する
/// </summary>
Application::~Application()
{	

}