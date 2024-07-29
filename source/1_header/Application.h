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

/// <summary>
/// FBXモデルの頂点用構造体
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//座標
	XMFLOAT3 normal;					//法線
	XMFLOAT2 uv;						//UV座標
	XMFLOAT3 tangent;					//正接
	XMFLOAT4 color;						//頂点カラー
	XMUINT4 ids;						//ボーンID
	XMFLOAT4 weights;					//ボーンの影響値
};

class Dx12Wrapper;
class EffectManager;
class InputManager;
class Package;
class PlayScene;
class SoundManager;
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

	//Dx12Wrapperインスタンス
	shared_ptr<Dx12Wrapper> _dx12;								
	//SoundManagerインスタンス
	shared_ptr<SoundManager> _sound;								
	//EffectManagerインスタンス
	shared_ptr<EffectManager> _effect;								
	
	//PlaySceneインスタンス
	shared_ptr<PlayScene> _play;									

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

	//アプリケーションを終了する
	void ExitApp();									

	//レンダーターゲットのフリップ間隔を返す関数
	int GetInterval()const;							
	//1秒間のフレームレートを返す関数
	int GetRate()const;								

	//デストラクタ
	~Application();									
};
