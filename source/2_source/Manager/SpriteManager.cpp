#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Manager/SpriteManager.h"
#include "Package/Package.h"
#include "Wrapper/Dx12Wrapper.h"

const int LOADING_WIDTH = 480;		//ロード画面で表示するアイコンの幅
const int LOADING_HEIGHT = 270;		//ロード画面で表示するアイコンの高さ

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="width">画面幅</param>
/// <param name="height">画面高さ</param>
SpriteManager::SpriteManager(Dx12Wrapper& dx12, LONG width, LONG height):_dx12(dx12),_width(width),_height(height)
{
	CreateSpriteRS();																				//Sprite用ルートシグネチャを作成
	InitSpriteDevices();																			//Sprite用オブジェクトを初期化

	AdjustSpriteRect();

	_incrementSize =																				//差分のサイズを取得
		_dx12.Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateUIBufferView(L"Asset/image/loading/1.png", "load_1");										//ロード画面に回転するアイコン
	CreateUIBufferView(L"Asset/image/loading/2.png", "load_2");
	CreateUIBufferView(L"Asset/image/loading/3.png", "load_3");					
	CreateUIBufferView(L"Asset/image/loading/4.png", "load_4");					
	CreateUIBufferView(L"Asset/image/loading/5.png", "load_5");					
	CreateUIBufferView(L"Asset/image/loading/6.png", "load_6");					
	CreateUIBufferView(L"Asset/image/loading/7.png", "load_7");					
	CreateUIBufferView(L"Asset/image/loading/8.png", "load_8");
}

/// <summary>
/// SpriteBatch向けのルートシグネチャ・シェーダーを作成する関数
/// </summary>
/// <returns>処理が成功したかどうか</returns>
HRESULT
SpriteManager::CreateSpriteRS()
{
	CD3DX12_DESCRIPTOR_RANGE spriteTblRange[2] = {};					//ディスクリプタレンジ(SRV用)
	spriteTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	spriteTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	
	CD3DX12_ROOT_PARAMETER spriteRootParam[3] = {};						//ルートパラメータ(SRV,CBV用)
	spriteRootParam[0].InitAsDescriptorTable(
		1, &spriteTblRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootParam[1].InitAsConstantBufferView(						//[1]はConstantBufferViewとして初期化
		0, 0, D3D12_SHADER_VISIBILITY_ALL);
	spriteRootParam[2].InitAsDescriptorTable(
		1, &spriteTblRange[1], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};						//サンプラー
	samplerDesc.Init(0);												//初期化
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//ピクセルシェーダーから見えるよう設定

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};							//ルートシグネチャ作成用構造体
	rsDesc.Init(3,spriteRootParam,1,&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* rsBlob = nullptr;											//ルートシグネチャの初期化
	ID3DBlob* errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rsBlob,
		&errorBlob);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	rsBlob->Release();													//使わないデータを開放

	result = _dx12.Device()->CreateRootSignature(						//ルートシグネチャ作成
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(_spriteRS.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// Sprite向けオブジェクトを初期化する関数
/// </summary>
void
SpriteManager::InitSpriteDevices()
{
	_gmemory = make_unique<GraphicsMemory>(_dx12.Device());								//グラフィックスメモリの初期化

	ResourceUploadBatch resUploadBatch(_dx12.Device());									//スプライト表示用オブジェクトの初期化
	resUploadBatch.Begin();

	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);		//レンダーターゲットステート

	unique_ptr<CommonStates> _states = make_unique<CommonStates>(_dx12.Device());		//サンプラーを取得するためStateオブジェクトを初期化
	D3D12_GPU_DESCRIPTOR_HANDLE wrap = _states->AnisotropicWrap();						//GPUハンドル

	SpriteBatchPipelineStateDescription psd(rtState, &CommonStates::NonPremultiplied);

	_spriteBatch = make_unique<SpriteBatch>(_dx12.Device(), resUploadBatch, psd);		//スプライト表示用オブジェクト

	_batch = make_unique<PrimitiveBatch<VertexPositionColor>>(_dx12.Device());

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_NONE,false,D3D12_DEFAULT_DEPTH_BIAS,D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,true,false,true,
		0,D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	EffectPipelineStateDescription ePsd
	(
		&VertexPositionColor::InputLayout,
		CommonStates::Opaque,
		CommonStates::DepthDefault,
		rastDesc,
		rtState,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
	);

	_effect = make_unique<BasicEffect>(_dx12.Device(), EffectFlags::VertexColor, ePsd);

	_world = SimpleMath::Matrix::Identity;
	_view = _dx12.ViewMatrix();
	_proj = _dx12.ProjMatrix();

	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//フォント表示用オブジェクトの初期化		
	_dx12.CreateDescriptorHeap(_heapForSpriteFont,										//フォント・画像表示用ヒープ
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,256, 
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	_tmpCPUHandle = _heapForSpriteFont->GetCPUDescriptorHandleForHeapStart();			//ヒープハンドル(CPU)
	_tmpGPUHandle = _heapForSpriteFont->GetGPUDescriptorHandleForHeapStart();			//ヒープハンドル(GPU)

	auto future = resUploadBatch.End(_dx12.CommandQueue());								//GPU側へ転送

	_dx12.WaitForCommandQueue();														//GPUが使用可能になるまで待機
	future.wait();

	_spriteBatch->SetViewport(*_dx12.ViewPort());										//スプライト表示用オブジェクトをビューポートへ登録

	return;
}

/// <summary>
/// アクターの当たり判定を描画する関数
/// </summary>
/// <param name="collider">当たり判定のポインタ</param>
void
SpriteManager::ColliderDraw(const shared_ptr<BoxCollider> collider)
{
	if (collider != nullptr)
	{
		auto v_d1 = VertexPositionColor(collider->Vertices()[0], { 0.f,1.0f,0.0f,.10f });
		auto v_d2 = VertexPositionColor(collider->Vertices()[1], { 0.f,1.0f,0.0f,.10f });
		auto v_d3 = VertexPositionColor(collider->Vertices()[2], { 0.f,1.0f,0.0f,.10f });
		auto v_d4 = VertexPositionColor(collider->Vertices()[3], { 0.f,1.0f,0.0f,.10f });

		_batch->DrawLine(v_d1, v_d2);
		_batch->DrawLine(v_d2, v_d3);
		_batch->DrawLine(v_d3, v_d4);
		_batch->DrawLine(v_d4, v_d1);

		auto v_u1 = VertexPositionColor(collider->Vertices()[4], { 0.f,1.0f,0.0f,.10f });
		auto v_u2 = VertexPositionColor(collider->Vertices()[5], { 0.f,1.0f,0.0f,.10f });
		auto v_u3 = VertexPositionColor(collider->Vertices()[6], { 0.f,1.0f,0.0f,.10f });
		auto v_u4 = VertexPositionColor(collider->Vertices()[7], { 0.f,1.0f,0.0f,.10f });

		_batch->DrawLine(v_u1, v_u2);
		_batch->DrawLine(v_u2, v_u3);
		_batch->DrawLine(v_u3, v_u4);
		_batch->DrawLine(v_u4, v_u1);

		_batch->DrawLine(v_d1, v_u1);
		_batch->DrawLine(v_d2, v_u2);
		_batch->DrawLine(v_d3, v_u3);
		_batch->DrawLine(v_d4, v_u4);
	}
}

/// <summary>
/// 画像のバッファー・ビューを作成する関数
/// </summary>
/// <param name="path">画像のパス</param>
/// <param name="key">連想配列のキー</param>
/// <returns>処理が成功したかどうか</returns>
HRESULT
SpriteManager::CreateUIBufferView(const wchar_t* path,string key)
{
	TexMetadata meta = {};															//UI画像読み込み用データ
	ScratchImage scratch = {};

	auto ext = FileExtension(path);													//拡張子を取得

	auto result = _dx12._loadLambdaTable[ToString(ext)](path, &meta, scratch);		//画像データの読み込み
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	auto img = scratch.GetImage(0, 0, 0);											//生データを取得

	DXGI_FORMAT format = meta.format;												//フォーマット
	size_t width = meta.width;														//幅
	size_t height = meta.height;													//高さ
	UINT16 arraySize = static_cast<UINT16>(meta.arraySize);							//テクスチャサイズ
	UINT16 mipLevels = static_cast<UINT16>(meta.mipLevels);		
	void* pixels = img->pixels;
	UINT rowPitch = static_cast<UINT>(img->rowPitch);
	UINT slicePitch = static_cast<UINT>(img->slicePitch);

	ID3D12Resource* tmpUIBuff = nullptr;											//画像データ書き込み用バッファ

	auto uiResDesc = CD3DX12_RESOURCE_DESC::Tex2D(									//リソース作成用データ
		format,
		(UINT)width,
		(UINT)height,
		arraySize,
		(UINT)mipLevels);

	result = _dx12.Device()->CreateCommittedResource(								//リソース作成
		&_writeHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&uiResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&tmpUIBuff));
	if (FAILED(result))
	{
		assert(0);
		return result;;
	}

	result = tmpUIBuff->WriteToSubresource(0,										//画像情報を書き込み
		nullptr,
		pixels,
		rowPitch,
		slicePitch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_tmpCPUHandle.ptr += _incrementSize;											//CPUハンドルをズラす
	_tmpGPUHandle.ptr += _incrementSize;											//GPUハンドルをズラす

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};									//UIビュー用構造体の作成
	srvDesc.Format = tmpUIBuff->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	_dx12.Device()->CreateShaderResourceView(tmpUIBuff, &srvDesc, _tmpCPUHandle);	//ビュー作成

	_GPUHandles[key] = _tmpGPUHandle;												//GPUハンドルを連想配列に格納

	return result;
}

/// <summary>
/// 画面サイズ変更時、矩形を調整する関数
/// </summary>
void
SpriteManager::AdjustSpriteRect()
{
	_loadingRect = { LOADING_WIDTH,LOADING_HEIGHT,_width - LOADING_WIDTH,_height - LOADING_HEIGHT };	//ロード画面の設定

	AdjustWindowRect(&_loadingRect, WS_OVERLAPPEDWINDOW, false);		
}

/// <summary>
/// ローディング画面での画像を描画する関数
/// </summary>
void
SpriteManager::LoadingDraw()
{
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };					//ヒープをコマンドリストにセット
	_dx12.CommandList()->SetDescriptorHeaps(1, heap);

	_spriteBatch->Begin(_dx12.CommandList());										//バッチをセット

	int count = ((clock() / static_cast<int>(60 * 1.4)) % 8) + 1;					//現在の時間に応じて表示する画像を変え、ロードアイコンのアニメーションを表現
	_spriteBatch->Draw(_GPUHandles["load_" + to_string(count)], XMUINT2(1, 1),
		_loadingRect, Colors::White);

	_spriteBatch->End();															//バッチを解除
}

/// <summary>
/// グリッドを描画する関数
/// </summary>
void
SpriteManager::GridDraw()
{
	_view = _dx12.ViewMatrix();											//ビュー行列を取得
	_proj = _dx12.ProjMatrix();											//プロジェクション行列を取得

	_effect->SetWorld(_world);											//ワールド・ビュー・プロジェクション行列をセット
	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	_effect->Apply(_dx12.CommandList());								//コマンドリストにグリッドを登録

	_batch->Begin(_dx12.CommandList());

	VertexPositionColor v1(_origin - _XAxis, { 0.75f,0.0f,0.0f,.10f });		//X軸のグリッドの描画
	VertexPositionColor v2(_origin + _XAxis, { 0.75f,0.0f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	v1 = VertexPositionColor(_origin - _YAxis, { 0.f,0.75f,0.0f,.10f });	//Y軸のグリッドの描画
	v2 = VertexPositionColor(_origin + _YAxis, { 0.f,0.75f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	v1 = VertexPositionColor(_origin - _ZAxis, { 0.0f,0.0f,0.75f,.10f });	//X軸のグリッドの描画
	v2 = VertexPositionColor(_origin + _ZAxis, { 0.0f,0.0f,0.75f,.10f });
	_batch->DrawLine(v1, v2);

	for (const auto& actor : _actorAndObjs)
	{
		if (actor != nullptr)ColliderDraw(actor->Collider());
	}

	_batch->End();
}

/// <summary>
/// グラフィックスメモリをコマンドキューにセットする関数
/// </summary>
void
SpriteManager::Commit()
{
	_gmemory->Commit(_dx12.CommandQueue());
}

/// <summary>
/// アクターを設定する関数
/// </summary>
/// <param name="actor">アクター</param>
void
SpriteManager::SetActors(vector<shared_ptr<FBXBase>> actorAndObjects)
{
	_actorAndObjs = actorAndObjects;
}