#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"
#include "Wrapper/SphericalCoordinates.h"
						
//視点→注視点のベクトルをXZ平面に制限するためのベクトル
const XMVECTOR XZ_PLANE = XMVectorSet(1.0f,0.0f,1.0f,0.0f);	
//フェードイン・アウトにかける時間
const float FADE_TIME = 1.0f;
//半径の初期値
const int INIT_RAD = 300;

/// <summary>
/// デバッグ用レイヤーを初期化する関数
/// </summary>
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(
		IID_PPV_ARGS(&debugLayer));
		
	debugLayer->EnableDebugLayer();				//デバッグレイヤーを有効化する
	debugLayer->Release();						//有効化したらインターフェイスを開放する
}

/// <summary>
/// デバイス関連を初期化する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::InitializeDXGIDevice()
{
	//Debugかそうでないかでファクトリーの作成関数を変える
#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG
	
	D3D_FEATURE_LEVEL levels[] = {												//フィーチャーレベルの配列を初期化
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	vector<IDXGIAdapter*> adapters;												//列挙したアダプターを格納
	IDXGIAdapter* tmpAdapter = nullptr;											//アダプター

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i	//ファクトリー内の全アダプターをベクトルに格納
		)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)													//格納した全アダプターに対しループ
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);													//アダプターの説明を記述する構造体を取得

		wstring strDesc = adesc.Description;									//アダプターの名前を取得

		if (strDesc.find(L"NVIDIA") != string::npos)							//アダプターの名前が特定の名前と一致したらループ終了
		{
			tmpAdapter = adpt;													//デバイス作成に使用するアダプターを決定
			break;
		}
	}

	for (auto lv : levels)														//初期化したフィーチャーレベルに対しループ
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)		//デバイスを作成できるフィーチャーレベルを探す
		{
			result = S_OK;
			break;																//生成可能なバージョンが見つかったらループ終了
		}
	}
	return result;
}

/// <summary>
/// コマンド関連を初期化する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::InitializeCommand()
{
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,	//コマンドアロケータを作成
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,			//コマンドリストを作成
		_cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};									//コマンドキュー設定構造体を作成・設定
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc,							//コマンドキューを作成
		IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// スワップチェーンを作成する関数
/// </summary>
/// <param name="hwnd">ウィンドウ識別用データ</param>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};						//スワップチェーン設定用構造体・設定
	swapchainDesc.Width = _winSize.cx;
	swapchainDesc.Height = _winSize.cy;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;				
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = _dxgiFactory->CreateSwapChainForHwnd(				//スワップチェーン作成
		_cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

/// <summary>
/// レンダーターゲットを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT 
Dx12Wrapper::CreateRenderTargetsView()
{
	auto result = CreateDescriptorHeap(_rtvHeap,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,0,2,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};									//スワップチェーンの情報取得
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);							//バックバッファーの数をスワップチェーンのバッファー数に合わせる
		
	D3D12_CPU_DESCRIPTOR_HANDLE handle =								//ディスクリプタヒープの先頭アドレス(ハンドル)
		_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};							//レンダーターゲットビュー設定用構造体の作成・設定
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)			//各バックバッファーに対しループを回す
	{
		result = _swapchain->GetBuffer(													//スワップチェーンからデータを取得、バックバッファーに割り当てる
			idx, IID_PPV_ARGS(&_backBuffers[idx]));	
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		_dev->CreateRenderTargetView(													//バックバッファーを対象にRTVを作成
			_backBuffers[idx],
			&rtvDesc,
			handle);

		handle.ptr +=																	//ハンドルの位置をずらす
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};													//スワップチェーン情報を取得（上記のswcDescとは別）
	result = _swapchain->GetDesc1(&desc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_viewPort.reset(new CD3DX12_VIEWPORT(_backBuffers[0]));								//スワップチェーンの情報を元にビューポートを初期化
	_rect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));						//同じくシザー矩形を初期化

	return result;
}

/// <summary>
/// ビュープロジェクション用ビューを作成
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT 
Dx12Wrapper::CreateSceneView()
{
	auto sceneResDesc = 
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + _constSize) & ~_constSize);

	CreateResource(_sceneConstBuff,_uploadHeapProp,										//定数バッファー作成
		sceneResDesc,D3D12_RESOURCE_STATE_GENERIC_READ);

	_mappedScene = nullptr;																//定数バッファーに対し書き込めるようにする
	auto result = _sceneConstBuff->Map(0,nullptr,(void**)&_mappedScene);

	result = CreateDescriptorHeap(_sceneDescHeap,										//ディスクリプタヒープ作成
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,0,1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//定数バッファービュー設定用構造体の作成・設定
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_sceneConstBuff->GetDesc().Width;
	
	_dev->CreateConstantBufferView(														//定数バッファービューの作成
		&cbvDesc,
		_sceneDescHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// 深度ステンシルビューを作成を作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES depthHeapProp = {};											//深度ステンシルバッファー用ヒーププロパティを作成
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC depthResDesc = {};												//バッファー用リソースディスクを作成・設定
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = _winSize.cx;
	depthResDesc.Height = _winSize.cy;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue = {};												//クリアバリューの作成・設定
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = _dev->CreateCommittedResource(										//バッファーの作成
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));

	result = CreateDescriptorHeap(_dsvHeap,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,0,1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};											//バッファービュー設定用構造体の作成・設定
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	
	_dev->CreateDepthStencilView(														//ビューの作成
		_depthBuffer.Get(),&dsvDesc,_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// エフェクト適用を決めるデータ用のヒープ・バッファーを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateFactorBufferAndView()
{
	auto factorResDesc
		= CD3DX12_RESOURCE_DESC::Buffer((sizeof(Factor) + _constSize) & ~_constSize);	//リソースディスク作成

	auto result = CreateResource(_factorConstBuff,_writeHeapProp,						//バッファー作成
		factorResDesc,D3D12_RESOURCE_STATE_GENERIC_READ);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	result = _factorConstBuff->Map(0, nullptr, (void**)&_mappedFactor);					//バッファーへ書き込めるようにする
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	UpdateFade();

	result = CreateDescriptorHeap(_factorCBVHeap,										//ディスクリプタヒープ作成
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,0,1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//定数バッファービュー設定用構造体の作成・設定
	cbvDesc.BufferLocation = _factorConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_factorConstBuff->GetDesc().Width;

	_dev->CreateConstantBufferView(														//定数バッファービューの作成
		&cbvDesc,
		_factorCBVHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// テクスチャロード用テーブルを作成する関数
/// ファイル名に含まれている拡張子に応じて実行する関数を変える
/// </summary>
void
Dx12Wrapper::CreateTextureLoaderTable()
{
	_loadLambdaTable["sph"]													//sph,spa,png,jpg拡張子をkeyに、LoadFromWICFile関数をvalueにする
		= _loadLambdaTable["spa"]
		= _loadLambdaTable["bmp"]
		= _loadLambdaTable["png"]
		= _loadLambdaTable["jpg"]
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromWICFile(path.c_str(), 0, meta, img);
	};

	_loadLambdaTable["tga"]													//tga拡張子をkeyに、LoadFromTGAFile関数をvalueにする
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};

	_loadLambdaTable["dds"]													//dds拡張子をkeyに、LoadFromDDSFile関数をvalueにする
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), 0, meta, img);
	};
}

/// <summary>
/// 視点、注視点のベクトルを初期化する関数
/// </summary>
void
Dx12Wrapper::InitVector()
{
	_eye = _initEye;
	_target = _initTarget;
}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="deltaTime">1フレーム当たりの秒数</param>
Dx12Wrapper::Dx12Wrapper() :
	 _perspective(true), _up(0, 1, 0),_light(0.0f,1.0f,0.0f), _fade(1.0f),_initEye(0, 50, INIT_RAD),_initTarget(0, 50, 0)
{

}

/// <summary>
/// デストラクタ
/// </summary>
Dx12Wrapper::~Dx12Wrapper()
{

}

/// <summary>
/// シングルトンを返す関数
/// </summary>
/// <returns>シングルトン</returns>
Dx12Wrapper&
Dx12Wrapper::Instance()
{
	static Dx12Wrapper instance;
	return instance;
}

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="deltaTime">フレーム間の経過時間</param>
void
Dx12Wrapper::Init(HWND hwnd, float deltaTime)
{
#ifdef _DEBUG
	//デバッグ用レイヤーを起動
	EnableDebugLayer();													
#endif 
	//ウィンドウのサイズを取得
	_winSize = Application::Instance().GetWindowSize();
	//フレームレートを取得
	_rate = Application::Instance().GetRate();
	//フレーム間の時間の差分を取得
	_deltaTime = deltaTime;

	//デバイス関連を初期化
	InitializeDXGIDevice();

	//コマンド関連を初期化
	InitializeCommand();

	CreateSwapChain(hwnd);												//スワップチェーンを作成

	CreateRenderTargetsView();											//レンダーターゲットを作成

	InitVector();														//視点、注視点のベクトルを初期化

	CreateSceneView();													//ビュープロジェクション用ビューを作成

	CreateTextureLoaderTable();											//テクスチャロード用テーブルを作成

	CreateDepthStencilView();											//深度ステンシルビューを作成

	CreateFactorBufferAndView();										//フェードイン／アウト用バッファー・ビューを作成

	_dev->CreateFence(													//フェンスを作成
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	//_effect.reset(new EffectManager(*this));							//EffectManagerインスタンスを初期化
}

/// <summary>
/// リソースを遷移させる関数
/// </summary>
/// <param name="resource">遷移させたいリソース</param>
/// <param name="before">遷移前のステート</param>
/// <param name="after">遷移後のステート</param>
void
Dx12Wrapper::BarrierTransition(
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after)
{
	_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);	//変数を設定

	//リソースを遷移させる
	_cmdList->ResourceBarrier(1, &_barrier);
}

/// <summary>
/// XZ平面に制限した視点座標から注視点座標へのベクトルを取得する関数
/// </summary>
/// <returns>ベクトル</returns>
XMVECTOR
Dx12Wrapper::GetXZVecEyeToTarget()const
{
	XMVECTOR vec = XMVectorSet
	(_target.x - _eye.x, _target.y - _eye.y, _target.z - _eye.z, 0);	//まず視点座標から注視点座標へのベクトルを取得

	vec = XMVectorMultiply(vec, XZ_PLANE);								//Y成分を0にし、ベクトルをXZ平面に制限

	XMVECTOR ret = XMVector3Normalize(vec);								//ベクトルを正規化
	
	return ret;
}

/// <summary>
/// 視点→注視点のベクトルに対し右に垂直なベクトルを返す関数
/// </summary>
/// <returns>垂直ベクトル</returns>
XMVECTOR
Dx12Wrapper::GetRightVector()const
{
	XMVECTOR eyeToTrgtVec = GetXZVecEyeToTarget();						//視点→注視点のベクトルを取得
	XMVECTOR upVec = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);			//上ベクトルを取得

	XMVECTOR cross = XMVector3Cross(eyeToTrgtVec, upVec);				//2ベクトルの外積を求める

	return cross;
}

/// <summary>
/// フレーム間の経過時間を返す関数
/// </summary>
/// <returns>フレーム間の経過時間</returns>
float 
Dx12Wrapper::GetDeltaTime()const
{
	return _deltaTime;
}

/// <summary>
/// カメラを近付ける・遠ざける関数
/// </summary>
/// <param name="x">移動距離</param>
void
Dx12Wrapper::ScalingCoordinates(int x)
{
	_eye = XMFLOAT3
	(
		_coordinates->Scaling(x * _deltaTime).ToCartesian().x + _target.x,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().y + _target.y,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().z + _target.z
	);
}

/// <summary>
/// カメラを平行移動させる関数
/// </summary>
/// <param name="dir">平行移動させる方向</param>
/// <param name="value">移動距離</param>
void
Dx12Wrapper::TranslateCoordinates(XMVECTOR vec)
{
	_target.x += vec.m128_f32[0] * _deltaTime * 300.0f;
	_target.y += vec.m128_f32[1] * _deltaTime * 300.0f;
	_target.z += vec.m128_f32[2] * _deltaTime * 300.0f;

	//その後カメラ座標を更新
	_eye =
		XMFLOAT3
		(
			_coordinates->ToCartesian().x + _target.x,
			_coordinates->ToCartesian().y + _target.y,
			_coordinates->ToCartesian().z + _target.z
		);
}

/// <summary>
/// SphericalCoordinates上のカメラ座標を回転させる関数
/// </summary>
/// <param name="deg">方位角か仰角か決める</param>
/// <param name="value">上か下か、右か左か</param>
void
Dx12Wrapper::RotateCoordinates(Degree deg, float value)
{
	switch (deg)
	{
	case Degree::Azimth:	//方位角の方向に回転させる
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().x + _target.x,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().y + _target.y,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().z + _target.z
		);
		break;
	case Degree::Elevation:	//仰角の方向に回転させる
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().x + _target.x,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().y + _target.y,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().z + _target.z
		);
		break;
	default:
		break;
	}
}

/// <summary>
/// カメラの位置を初期化する関数
/// </summary>
/// <param name="azimth">方位角</param>
/// <param name="elevation">仰角</param>
void
Dx12Wrapper::ResetCoordinates(float azimth, float elevation)
{
	InitVector();											//視点、注視点をリセット

	_coordinates.reset(new SphericalCoordinates());			//球面座標を扱うSphericalCoordinatesインスタンスを作成
	_coordinates->SetRadius(INIT_RAD);						//半径
	_coordinates->SetAzimth(azimth);						//方位角
	_coordinates->SetElevation(elevation);					//仰角

	_eye = XMFLOAT3											//視点座標を設定
	(
		_coordinates->ToCartesian().x + _target.x,
		_coordinates->ToCartesian().y + _target.y,
		_coordinates->ToCartesian().z + _target.z
	);
}

/// <summary>
/// ビュープロジェクション用ビューをセットする関数
/// </summary>
void
Dx12Wrapper::SetScene()
{
	_mappedScene->view = XMMatrixLookAtLH(										//行列・座標を書き込む
		XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));

	if (_perspective)															//透視投影
	{
		_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
			static_cast<float>(_winSize.cx) / static_cast<float>(_winSize.cy),
			0.1f,
			1000.0f
		);
	}
	else																		//平行投影
	{
		_currentRad = _coordinates->GetRadius();

		_mappedScene->proj = XMMatrixOrthographicLH(
			static_cast<float>(_winSize.cx) / (XM_PI * (INIT_RAD / _currentRad)),
			static_cast<float>(_winSize.cy) / (XM_PI * (INIT_RAD / _currentRad)),
			0.1f, 1000.0f);
	}

	_mappedScene->light = _light;
	_mappedScene->eye = _eye;

	ID3D12DescriptorHeap* sceneHeaps[] = { _sceneDescHeap.Get() };				//ディスクリプタヒープをコマンドリストにセット
	_cmdList->SetDescriptorHeaps(1, sceneHeaps);

	_cmdList->SetGraphicsRootDescriptorTable(									//ヒープのハンドルをルートパラメータと関連付け
		0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// ゲーム画面用リソースの遷移(PRESENT→RENDER_TARGET)・RTVのセットを実行する関数
/// </summary>
void
Dx12Wrapper::BeginGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();									//現在のバックバッファーのインデックスを取得

	BarrierTransition(																		//ゲーム画面用リソースをRENDER_TARGETに遷移
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart();								//インデックスに応じてRTVヒープのハンドルをズラす
	rtvH.ptr += 
		bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();

	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);									//RTV・DSVヒープをセット

	float clearColor[4] = { 0.2f,0.2f,0.2f,1.0f };											//背景色を設定

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);							//RTVを初期化
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);		//DSVも初期化
}

/// <summary>
/// ゲーム用リソースの遷移(RENDER_TARGET→STATE_PRESENT)を実行する関数
/// </summary>
void
Dx12Wrapper::EndGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();		//現在のバックバッファーのインデックスを取得

	BarrierTransition(											//ゲーム画面用リソースをPRESENTに遷移
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT);

	_cmdList->Close();											//コマンドリストのクローズ

	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };			//コマンドリストの実行
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	WaitForCommandQueue();										//処理の同期待ちを行う

	_cmdAllocator->Reset();										//キューのクリア
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);				//再度コマンドリストを貯める準備
}
/// <summary>
/// 処理の同期待ちを行う関数
/// </summary>
void
Dx12Wrapper::WaitForCommandQueue()
{
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);					//フェンス値を更新

	if (_fence->GetCompletedValue() != _fenceVal)					//CPUとGPUのフェンス値を比較し、一致するまで処理を待ち合わせる
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	//イベントハンドルを取得？

		_fence->SetEventOnCompletion(_fenceVal, event);				//フェンス値が_fenceValになった時イベントを通知

		WaitForSingleObject(event, INFINITE);						//イベントが発生するまで待ち続ける

		CloseHandle(event);											//イベントを閉じる
	}
}

/// <summary>
/// ルートシグネチャ・パイプライン・描画方法をセットする関数
/// </summary>
/// <param name="rootSignature">ルートシグネチャ</param>
/// <param name="pipeline">パイプライン</param>
/// <param name="topology">描画方法</param>
void
Dx12Wrapper::SetPipelines(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipeline, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	_cmdList->SetGraphicsRootSignature(rootSignature);	//ルートシグネチャをセット
	_cmdList->SetPipelineState(pipeline);				//パイプラインをセット
	_cmdList->IASetPrimitiveTopology(topology);			//描画方法を設定
}

/// <summary>
/// フェードイン／アウトデータをシェーダーに反映する関数
/// </summary>
void
Dx12Wrapper::UpdateFade()
{
	_mappedFactor->fade = clamp(_fade, 0.0f, 1.0f);		//α値を0以上1以下に制限
}

/// <summary>
/// フェードイン／アウトを実行する関数
/// </summary>
/// <param name="start">フェード値の初期値</param>
/// <param name="end">フェード値の最終値</param>
/// <param name="func">フェード完了時に実行したい処理</param>
void
Dx12Wrapper::Fade(float start, float end)
{
	_mtx.lock();

	float t = 0.0f;							//時間
	_start = start;							//フェード値の初期値
	_end = end;								//フェード値の最終値

	_rate *= FADE_TIME;

	//線形補間処理
	for (int i = 0; i <= _rate; i++)
	{
		_fade = lerp(_start, _end, t);		//フェード値を線形補間

		t += _deltaTime / FADE_TIME;		//加算

		Sleep(1);							//処理を待つ
	}

	_mtx.unlock();
}

/// <summary>
/// バッファーを作成する関数
/// </summary>
/// <param name="buff">作成したバッファーを格納するポインタ</param>
/// <param name="heapProp">ヒーププロパティ</param>
/// <param name="resDesc">リソースディスク</param>
/// <param name="resState">リソースステート</param>
HRESULT
Dx12Wrapper::CreateResource(
	ComPtr<ID3D12Resource>& buff,CD3DX12_HEAP_PROPERTIES heapProp,
	CD3DX12_RESOURCE_DESC resDesc,	D3D12_RESOURCE_STATES resState
)
{
	auto ret = _dev->CreateCommittedResource(	//定数バッファー(リソース)を作成
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		resState,
		nullptr,
		IID_PPV_ARGS(&buff));

	return ret;
}

/// <summary>
/// ディスクリプタヒープを作成する関数
/// </summary>
/// <param name="heap">ヒープを格納するポインタ</param>
/// <param name="type">ヒープタイプ</param>
/// <param name="nodeMask">ノードマスク</param>
/// <param name="numDescriptor">ディスクリプタ数</param>
/// <param name="flag">ヒープフラグ</param>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Dx12Wrapper::CreateDescriptorHeap(
	ComPtr<ID3D12DescriptorHeap>& heap,	D3D12_DESCRIPTOR_HEAP_TYPE type,
	UINT nodeMask,UINT numDescriptor,D3D12_DESCRIPTOR_HEAP_FLAGS flag)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};							//ディスクリプタヒープ設定構造体の作成・設定
	heapDesc.Type = type;
	heapDesc.NodeMask = nodeMask;
	heapDesc.NumDescriptors = numDescriptor;
	heapDesc.Flags = flag;

	auto result = _dev->CreateDescriptorHeap(							//ディスクリプタヒープの作成
		&heapDesc, IID_PPV_ARGS(&heap));

	return result;
}

/// <summary>
/// 右手系のビュー変換行列を返す関数
/// </summary>
/// <returns>行列</returns>
XMMATRIX
Dx12Wrapper::ViewMatrix()const
{
	XMFLOAT3 RHEye = XMFLOAT3(-_eye.x,_eye.y,_eye.z);
	XMFLOAT3 RHTarget = XMFLOAT3(-_target.x, _target.y, _target.z);

	return XMMatrixLookAtRH(XMLoadFloat3(&RHEye), XMLoadFloat3(&RHTarget), XMLoadFloat3(&_up));
}

/// <summary>
/// 右手系のプロジェクション変換行列を返す関数
/// </summary>
/// <returns>行列</returns>
XMMATRIX
Dx12Wrapper::ProjMatrix()const
{
	if (_perspective)
	{
		return XMMatrixPerspectiveFovRH(
			XM_PIDIV4,ViewPort()->Width / ViewPort()->Height,
			0.1f,
			1000.0f
		);
	}
	else
	{
		return XMMatrixOrthographicRH(
			static_cast<float>(_winSize.cx) / (XM_PI * (INIT_RAD / _currentRad)),
			static_cast<float>(_winSize.cy) / (XM_PI * (INIT_RAD / _currentRad)),
			0.1f, 1000.0f);
	}
}

/// <summary>
/// EffectManagerインスタンスを返す関数
/// </summary>
/// <returns>インスタンスのポインタ</returns>
EffectManager*
Dx12Wrapper::Effect()const
{
	return _effect.get();
}

/// <summary>
/// デバイスを返す関数
/// </summary>
/// <returns>デバイスのポインタ</returns>
ID3D12Device*
Dx12Wrapper::Device()const
{
	return _dev.Get();
}

/// <summary>
/// スワップチェーンを返す関数
/// </summary>
/// <returns>スワップチェーンのポインタ</returns>
IDXGISwapChain4*
Dx12Wrapper::Swapchain()const
{
	return _swapchain.Get();
}

/// <summary>
/// コマンドリストを返す関数
/// </summary>
/// <returns>コマンドリストのポインタ</returns>
ID3D12GraphicsCommandList*
Dx12Wrapper::CommandList()const
{
	return _cmdList.Get();
}

/// <summary>
/// コマンドキューを返す関数
/// </summary>
/// <returns>コマンドキューのポインタ</returns>
ID3D12CommandQueue*
Dx12Wrapper::CommandQueue()const
{
	return _cmdQueue.Get();
}

/// <summary>
/// バックバッファー（1枚目）を返す関数
/// </summary>
/// <returns></returns>
ID3D12Resource*
Dx12Wrapper::BackBuffer() const
{
	return _backBuffers[0];
}

/// <summary>
/// RTVヒープを返す関数
/// </summary>
/// <returns>RTVヒープ</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::RTVHeap() const
{
	return _rtvHeap.Get();
}

/// <summary>
/// 深度ステンシルヒープを返す関数
/// </summary>
/// <returns>深度ステンシルヒープ</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::DSVHeap() const
{
	return _dsvHeap.Get();
}

/// <summary>
/// 因数用ヒープを返す関数
/// </summary>
/// <returns>因数用ヒープ</returns>
ID3D12DescriptorHeap* 
Dx12Wrapper::FactorCBVHeap() const
{
	return _factorCBVHeap.Get();
}

/// <summary>
/// ビューポートを返す関数
/// </summary>
/// <returns>ビューポート</returns>
D3D12_VIEWPORT*
Dx12Wrapper::ViewPort() const
{
	return _viewPort.get();
}

/// <summary>
/// シザー矩形を返す関数
/// </summary>
/// <returns>シザー矩形</returns>
D3D12_RECT*
Dx12Wrapper::Rect() const
{
	return _rect.get();
}

/// <summary>
/// 視点座標を返す関数
/// </summary>
/// <returns>視点座標</returns>
XMVECTOR
Dx12Wrapper::Eye() const
{
	return XMLoadFloat3(&_eye);
}

/// <summary>
/// 透視投影かどうか返す関数
/// </summary>
/// <returns>真理値</returns>
bool
Dx12Wrapper::Perspective()const
{
	return _perspective;
}

/// <summary>
/// カメラを透視投影にするかどうか決める関数
/// </summary>
/// <param name="isPers">透視投影かどうか（trueなら透視、falseなら並行）</param>
void
Dx12Wrapper::SetPersOrOrtho(bool isPers)
{
	_perspective = isPers;
}