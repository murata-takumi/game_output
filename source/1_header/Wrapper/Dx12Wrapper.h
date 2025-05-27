#pragma once
#include "Includes.h"
#include "Functions.h"
#include "Vector3.h"

/// <summary>
/// カメラをどちらの方に回転させるか決定する列挙体
/// </summary>
enum Degree
{
	//方位角
	Azimth,
	//仰角
	Elevation,		
};

/// <summary>
/// カメラをどちらの方向へ平行移動させるか決定する列挙体
/// </summary>
enum Direction
{
	//水平方向
	Horizontal,	
	//垂直方向
	Vertical,	
	//奥行方向
	Depth,		
};

/// <summary>
/// 描画命令を実行するためのオブジェクト等を管理するクラス
/// </summary>
class EffectManager;
class SphericalCoordinates;
class SpriteManager;
class PeraRenderer;
class Renderer;
class ImGuiManager;
class Dx12Wrapper
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	//ウィンドウサイズ
	SIZE _winSize;												

	//エフェクト管理用インスタンス
	shared_ptr<EffectManager> _effect;					
	//UI管理インスタンス
	shared_ptr<PeraRenderer> _pera;						
	//カメラ座標用インスタンス
	shared_ptr<SphericalCoordinates> _coordinates;				

	//DXGI周り
	//ファクトリー
	ComPtr<IDXGIFactory6> _dxgiFactory;							
	//デバイス
	ComPtr<ID3D12Device> _dev;									

	//DX12(コマンド)周り
	//コマンドアロケータ
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;				
	//コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> _cmdList;					
	//コマンドキュー
	ComPtr<ID3D12CommandQueue> _cmdQueue;						
	//スワップチェーン
	ComPtr<IDXGISwapChain4> _swapchain;							

	//表示関連のバッファ回り
	//レンダーターゲット用バッファー
	vector<ID3D12Resource*> _backBuffers;						
	//レンダーターゲットビュー用ヒープ
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;						
	//深度ステンシル用バッファー
	ComPtr<ID3D12Resource> _depthBuffer;						
	//深度ステンシルビュー用ヒープ
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;						

	//ビューポート
	unique_ptr<D3D12_VIEWPORT> _viewPort = {};					
	//シザー矩形
	unique_ptr<D3D12_RECT> _rect = {};							

	//ビュープロジェクション用定数バッファ周り
	//ビュー・プロジェクション用バッファー
	ComPtr<ID3D12Resource> _sceneConstBuff;						
	//ビュー・プロジェクション用ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap;				

	//歪みデータ用バッファー
	ComPtr<ID3D12Resource> _factorConstBuff;
	//歪みデータ用ヒープ
	ComPtr<ID3D12DescriptorHeap> _factorCBVHeap;					

	//imguiで使用するヒープ
	ComPtr<ID3D12DescriptorHeap> _heapForImgui;					

	/// <summary>
	/// シェーダーにフェードイン／アウトデータを渡すための構造体及びインスタンス
	/// </summary>
	struct Factor
	{
		float fade;
	};
	//マップ用ポインタ
	Factor* _mappedFactor = nullptr;							

	/// <summary>
	/// スロットに流し込むデータの構造体及びインスタンス
	/// 各行列を別個に流し込むのではなく、構造体としてまとめる
	/// </summary>
	struct SceneData
	{
		XMMATRIX view;
		XMMATRIX proj;
		Vector3 light;
		Vector3 eye;
	};
	//マップ用ポインタ
	SceneData* _mappedScene;									

	//フェンス
	ComPtr<ID3D12Fence> _fence;									
	//初期化値
	UINT64 _fenceVal;											

	//リソースバリア用変数
	CD3DX12_RESOURCE_BARRIER _barrier;							

	//視点（カメラ）座標
	Vector3 _eye;
	//注視点座標
	Vector3 _target;
	//上座標
	Vector3 _up;
	//ライト座標
	Vector3 _light;

	//視点の初期座標
	Vector3 _initEye;											
	//注視点の初期座標
	Vector3 _initTarget;

	//排他制御用オブジェクト
	mutex _mtx;													

	//透視投影かどうか決める真理値
	bool _perspective;

	//現在の半径
	float _currentRad;
	//フレーム間の時間
	float _deltaTime;											
	//フェードイン・アウトに用いる変数
	float _fade;												
	//フェードイン・アウトで線形補完する際の初期値、最終値
	float _start, _end;											

	//フレームレート
	int _rate;													
		
	//テクスチャロード用ラムダ式を格納する型の定義
	using LoadLambda_t =										
		function<HRESULT(
			const wstring& path, TexMetadata*, ScratchImage&
		)>;

	//stringをkey,リソースをvalueとした連想配列
	map<string, ID3D12Resource*> _resourceTable;

	//コンストラクタ
	Dx12Wrapper();
	Dx12Wrapper(const Dx12Wrapper&) = delete;
	//デストラクタ
	~Dx12Wrapper();

	//デバイス関連を初期化する関数
	HRESULT InitializeDXGIDevice();								

	//コマンド関連を初期化する関数
	HRESULT InitializeCommand();								

	//スワップチェーンを作成する関数
	HRESULT CreateSwapChain(const HWND& hwnd);					

	//レンダーターゲットを作成する関数
	HRESULT CreateRenderTargetsView();							

	//ビュープロジェクション用ビューを作成する関数
	HRESULT CreateSceneView();									

	//深度ステンシルビューを作成する関数
	HRESULT CreateDepthStencilView();							

	//エフェクト適用を決めるデータ用のヒープ・バッファーを作成する関数
	HRESULT CreateFactorBufferAndView();						

	//テクスチャロート用テーブルを作成する関数
	void CreateTextureLoaderTable();							

	//視点、注視点のベクトルを初期化する関数
	void InitVector();											

public:
	//stringをkey,LoadLambda_tをvalueとした連想配列
	map<string, LoadLambda_t> _loadLambdaTable;							

	//シングルトンを返す関数
	static Dx12Wrapper& Instance();

	//初期化関数
	void Init(HWND hwnd, float deltatime);

	//リソースを遷移させる関数
	void BarrierTransition(												
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES before, 
		D3D12_RESOURCE_STATES after);

	//視点座標（カメラ座標）から注視点座標へのベクトルを取得する関数
	Vector3 GetXZVecEyeToTarget()const;
	//注視点へのベクトルの右方向のベクトルを取得する関数
	Vector3 GetRightVector()const;

	//前フレームと現フレームの時間の差分を取得する関数
	float GetDeltaTime()const;

	//カメラを近付ける・遠ざける関数
	void ScalingCoordinates(int x);								
	//カメラを移動させる関数
	void SetCoordinatesCenter(Vector3 vec);							
	//カメラを回転させる関数
	void RotateCoordinates(Degree deg, float value);					
	//カメラの位置を初期化する関数
	void ResetCoordinates(float azimth,float elevation);				

	//ビュープロジェクション用ビューをコマンドリストにセットする関数
	void SetScene();													

	//リソースの遷移(STATE_PRESENT→RENDER_TARGET)・RTVのセットを実行する関数
	void BeginGameDraw();												
	//リソースの遷移(RENDER_TARGET→STATE_PRESENT)を実行する関数
	void EndGameDraw();													

	//処理の同期待ちを行う関数
	void WaitForCommandQueue();											

	//ルートシグネチャ・パイプライン・描画方法をセットする関数
	void SetPipelines(ID3D12RootSignature* rootSignature,				
		ID3D12PipelineState* pipeline, 
		D3D12_PRIMITIVE_TOPOLOGY topology);

	//フェードイン・アウトデータをシェーダーに反映する関数
	void UpdateFade();													
	//フェードイン・アウトを実行する関数
	void Fade(float start,float end);									

	//バッファーを作成する関数
	HRESULT CreateResource(												
		ComPtr<ID3D12Resource>& buff,
		CD3DX12_HEAP_PROPERTIES heapProp,
		CD3DX12_RESOURCE_DESC resDesc,
		D3D12_RESOURCE_STATES resState
	);
	//ディスクリプタヒープを作成する関数
	HRESULT CreateDescriptorHeap(										
		ComPtr<ID3D12DescriptorHeap>& heap,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		UINT nodeMask,
		UINT numDescriptor,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag
	);

	//ビュー行列を返す関数
	XMMATRIX ViewMatrix()const;					
	//プロジェクション行列を返す関数
	XMMATRIX ProjMatrix()const;											

	//EffectManagerインスタンスを返す関数
	EffectManager* Effect() const;									

	//デバイスを返す関数
	ID3D12Device* Device() const;										
	//スワップチェーンを返す関数
	IDXGISwapChain4* Swapchain() const;		
	//コマンドリストを返す関数
	ID3D12GraphicsCommandList* CommandList() const;				
	//コマンドキューを返す関数
	ID3D12CommandQueue* CommandQueue() const;							
	//バックバッファー（1枚目）を返す関数
	ID3D12Resource* BackBuffer() const;									
	//RTVヒープを返す関数
	ID3D12DescriptorHeap* RTVHeap() const;								
	//深度ステンシルヒープを返す関数
	ID3D12DescriptorHeap* DSVHeap() const;								
	//因数用ヒープを返す関数
	ID3D12DescriptorHeap* FactorCBVHeap() const;						
	//ビューポートを返す関数
	D3D12_VIEWPORT* ViewPort() const;									
	//シザー矩形を返す関数
	D3D12_RECT* Rect() const;

	//視点座標を返す関数
	Vector3 Eye() const;

	//透視投影かどうか返す関数
	bool Perspective()const;
	//平行投影か透視投影か決める関数
	void SetPersOrOrtho(bool isPers);									
};

