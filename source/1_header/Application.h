#pragma once
#include "Includes.h"

#if _DEBUG

#pragma comment(lib,"assimp-vc143-mtd.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"LLGId.lib")

#else

#pragma comment(lib,"assimp-vc143-mt.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"LLGI.lib")

#endif // DEBUG


#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Imgui.lib")

/// <summary>
/// 読み込むSceneクラスを識別するための列挙型
/// </summary>
enum SceneNames
{
	Title,		//タイトルシーン
	Play,		//ゲームシーン
};

class Dx12Wrapper;
class EffectManager;
class InputManager;
class IScene;
class PlayScene;
class Package;
class SceneComponent;
class SoundManager;
class TitleScene;
/// <summary>
/// ゲームの初期化・更新・終了を管理するクラス
/// </summary>
class Application
{
private:
	//ウィンドウ作成時に必要な情報を格納
	WNDCLASSEX _windowClass;								
	//ウィンドウの識別に必要な値
	HWND _hwnd;													

	//EffectManagerインスタンス
	shared_ptr<EffectManager> _effect;								
	
	//現在実行しているシーン
	shared_ptr<IScene> _scene;
	//TitleSceneインスタンス
	shared_ptr<IScene> _title;
	//PlaySceneインスタンス
	shared_ptr<IScene> _play;

	//ゲーム内で使用する各種データを管理するインスタンス
	shared_ptr<Package> _package;									

	//フレーム間の経過時間
	float _deltaTime;				
	//1秒あたりのフレーム数
	int _rate;														
	//フリップ間隔
	int _interval;													

	//ゲーム用ウィンドウを作成する関数
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		

	//コンストラクタ
	Application();													
	//コンストラクタを外部から呼び出されないよう設定
	Application(const Application&) = delete;						

public:

	//メッセージ用構造体
	MSG _msg = {};									

	//インスタンスの参照を返す？
	static Application& Instance();					

	//初期化
	bool Init();									

	//ゲーム画面の描画
	void Run();										

	//ゲーム終了時の後始末
	void Terminate();								

	//ウィンドウサイズを返す
	SIZE GetWindowSize()const;	

	//シーンを切り替える
	void SetScene(shared_ptr<IScene> scene);
	//シーンの設定・終了時の処理・開始時の処理を実行
	void ChangeScene(SceneNames name);
	//アプリケーションを終了する
	void ExitApp();									

	//レンダーターゲットのフリップ間隔を返す関数
	int GetInterval()const;							
	//1秒間のフレームレートを返す関数
	int GetRate()const;								

	//デストラクタ
	~Application();									
};
