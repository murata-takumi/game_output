#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>
#include <DirectXTK12-master/Inc/SimpleMath.h>

#include <d3dcompiler.h>
#include <array>
#include <cstdlib>
#include <map>
#include <tchar.h>
#include <dxgi1_6.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

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

const float PI = 3.141592653f;		//円周率

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
	WNDCLASSEX _windowClass;										//ウィンドウ作成時に必要な情報を格納
	HWND _hwnd;														//ウィンドウの識別に必要な値

	shared_ptr<Dx12Wrapper> _dx12;									//Dx12Wrapperインスタンス
	shared_ptr<InputManager> _input;								//InputManagerインスタンス	
	shared_ptr<SoundManager> _sound;								//SoundManagerインスタンス
	shared_ptr<EffectManager> _effect;								//EffectManagerインスタンス
	
	shared_ptr<PlayScene> _play;									//PlaySceneインスタンス

	shared_ptr<Package> _package;									//ゲーム内で使用する各種データを管理するインスタンス

	float _deltaTime;												//フレーム間の経過時間

	int _rate;														//1秒あたりのフレーム数
	int _interval;													//フリップ間隔

	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		//ゲーム用ウィンドウを作成する関数

	Application();													//コンストラクタ
	Application(const Application&) = delete;						//コンストラクタを外部から呼び出されないよう設定

public:

	MSG _msg = {};									//メッセージ用構造体

	static Application& Instance();					//インスタンスの参照を返す？

	bool Init();									//初期化

	void Run();										//ゲーム画面の描画

	void Terminate();								//ゲーム終了時の後始末

	SIZE GetWindowSize()const;						//ウィンドウサイズを返す

	void ExitApp();									//アプリケーションを終了する

	int GetInterval()const;							//レンダーターゲットのフリップ間隔を返す関数
	int GetRate()const;								//1秒間のフレームレートを返す関数

	~Application();									//デストラクタ
};
