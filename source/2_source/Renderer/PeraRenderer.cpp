#include "Functions.h"
#include "Renderer/PeraRenderer.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
PeraRenderer::PeraRenderer()
{

}

/// <summary>
/// デストラクタ
/// 特に処理はしない
/// </summary>
PeraRenderer::~PeraRenderer()
{

}

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
PeraRenderer&
PeraRenderer::Instance()
{
	static PeraRenderer instance;
	return instance;
}

/// <summary>
/// 初期化
/// </summary>
void
PeraRenderer::Init()
{
	//ペラポリゴン用RT・RTV・SRVを作成
	CreatePeraResourcesAndView();							

	//ペラポリゴン用頂点バッファー・VBVを作成
	CreatePeraVertex();										

	//ペラポリゴン用パイプラインを作成
	CreatePeraPipeline();									
}

/// <summary>
/// ペラポリゴン用リソース・RTV・SRVを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraResourcesAndView()
{
	//元々使用していたリソース設定用構造体を利用
	auto resDesc = Dx12Wrapper::Instance().BackBuffer()->GetDesc();

	//クリア値はレンダリング時と同じ値を設定する必要がある
	float clsClr[4] = { 0.25f,0.25f,0.25f,1.0f };
	D3D12_CLEAR_VALUE clearValue =
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr);

	//リソース(レンダーターゲット)の作成
	auto result = Dx12Wrapper::Instance().Device()->CreateCommittedResource(
		&_defHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&_peraResource));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//RTV用ヒープ用構造体の設定
	auto heapDesc = Dx12Wrapper::Instance().RTVHeap()->GetDesc();
	heapDesc.NumDescriptors = 1;

	//RTV用ヒープの作成
	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(&_peraRTVHeap));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//RTV用構造体の設定
	D3D12_RENDER_TARGET_VIEW_DESC peraRTVDesc = {};
	peraRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	peraRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//RTVの作成
	Dx12Wrapper::Instance().Device()->CreateRenderTargetView(
		_peraResource.Get(),
		&peraRTVDesc,
		_peraRTVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//SRV用ヒープ用構造体の設定
	heapDesc.NumDescriptors = 1;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	//SRV用ヒープの作成
	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(
		&heapDesc, IID_PPV_ARGS(_peraSRVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	//SRV用構造体の作成
	D3D12_SHADER_RESOURCE_VIEW_DESC peraSRVDesc = {};
	peraSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	peraSRVDesc.Format = peraRTVDesc.Format;
	peraSRVDesc.Texture2D.MipLevels = 1;
	peraSRVDesc.Shader4ComponentMapping
		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//SRVの作成
	Dx12Wrapper::Instance().Device()->CreateShaderResourceView(
		_peraResource.Get(),
		&peraSRVDesc,
		_peraSRVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return result;
}

/// <summary>
/// ペラポリゴン用頂点バッファー・VBVを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraVertex()
{
	//ペラポリゴンを作成するための頂点構造体
	struct PeraVertex
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	//ペラポリゴンを構成する4つの頂点
	PeraVertex pv[] =
	{
		{{-1.0f,-1.0f,0.1f},{0.0f,1.0f}},
		{{-1.0f,1.0f,0.1f},{0.0f,0.0f}},
		{{1.0f,-1.0f,0.1f},{1.0f,1.0f}},
		{{1.0f,1.0f,0.1f},{1.0f,0.0f}}
	};

	//リソースのサイズを設定
	auto pvResDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));

	//ペラポリゴン用頂点バッファーの作成
	auto result = Dx12Wrapper::Instance().Device()->CreateCommittedResource(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&pvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_peraVB));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	//ペラポリゴンをGPU側に転送できるようにする
	PeraVertex* mappedPera = nullptr;
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	copy(begin(pv), end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	//頂点バッファービューの設定
	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	return result;
}

/// <summary>
/// ペラポリゴン用パイプラインを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
PeraRenderer::CreatePeraPipeline()
{
	//ペラポリゴン用頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC layout[2] =
	{
		//頂点座標
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		//UV座標
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	//ペラポリゴン用パイプライン用構造体の作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};
	//入力レイアウト数
	gpsDesc.InputLayout.NumElements = _countof(layout);
	//入力レイアウト設定
	gpsDesc.InputLayout.pInputElementDescs = layout;

	//エラー確認用データ
	ID3DBlob* errorBlob = nullptr;

	//関数の返り値をあらかじめ設定
	auto result = S_OK;

	//Debugビルド時はファイルから直接読み込む
#ifdef _DEBUG																			

	//頂点シェーダー用データ
	ID3DBlob* vs = nullptr;
	//ピクセルシェーダー用データ
	ID3DBlob* ps = nullptr;

	//頂点シェーダー読み込み
	result = D3DCompileFromFile(L"3_shader/Pera/PeraVertex.hlsl", nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraVS", "vs_5_0", 0, 0,
		&vs,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	//ピクセルシェーダー読み込み
	result = D3DCompileFromFile(L"3_shader/Pera/PeraPixel.hlsl", nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraPS", "ps_5_0", 0, 0,
		&ps,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}
	//パイプライン用構造体に頂点・ピクセルシェーダーを設定
	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vs);
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps);

	//Releaseビルド時はコンパイル済みシェーダーを読み込む
#else																					

	//頂点シェーダーのバイナリとメモリサイズを設定
	gpsDesc.VS.pShaderBytecode = g_PeraVS;
	gpsDesc.VS.BytecodeLength = sizeof(g_PeraVS);
	//ピクセルシェーダーも同様に
	gpsDesc.PS.pShaderBytecode = g_PeraPS;
	gpsDesc.PS.BytecodeLength = sizeof(g_PeraPS);

#endif

	//パイプラインステートの設定
	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	//ディスクリプタレンジ
	//画面テクスチャ[t0]
	D3D12_DESCRIPTOR_RANGE range[2] = {};
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;

	//定数スロット[b0]
	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	range[1].BaseShaderRegister = 0;
	range[1].NumDescriptors = 1;

	//ルートパラメータ
	D3D12_ROOT_PARAMETER rp[2] = {};
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;

	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	//サンプラー
	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャ設定構造体の作成
	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 2;
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//ルートシグネチャ用データ
	ID3DBlob* rsBlob = nullptr;

	//ルートシグネチャ用データの初期化
	result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rsBlob,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	//ルートシグネチャの作成
	result = Dx12Wrapper::Instance().Device()->CreateRootSignature(
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(&_peraRS));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	//不要になったデータを開放
	rsBlob->Release();

	//パイプラインへルートシグネチャを登録
	gpsDesc.pRootSignature = _peraRS.Get();

	//パイプラインの作成
	result = Dx12Wrapper::Instance().Device()->CreateGraphicsPipelineState(
		&gpsDesc, IID_PPV_ARGS(&_peraPipeline));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// ペラポリゴン用リソースの遷移(SHADER_RESOURCE→RENDER_TARGET)・RTVのセットを実行する関数
/// </summary>
void
PeraRenderer::BeginPeraDraw()
{
	//ペラポリゴン用リソースをRENDER_TARGETに遷移
	Dx12Wrapper::Instance().BarrierTransition(																
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//ペラポリゴン用RTV・SRVヒープのハンドル
	auto rtvH = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();	
	//DSVヒープのハンドル
	auto dsvH = Dx12Wrapper::Instance().DSVHeap()->GetCPUDescriptorHandleForHeapStart();						

	//RTV・DSVヒープのハンドルをコマンドリストに設定
	Dx12Wrapper::Instance().CommandList()->OMSetRenderTargets(1, &rtvH, false, &dsvH);						

	//背景色を設定
	float clearColor[4] = { 0.25f,0.25f,0.25f,1.0f };										

	//RTVをクリア
	Dx12Wrapper::Instance().CommandList()->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);				
	//DSVをクリア
	Dx12Wrapper::Instance().CommandList()->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	//ビューポートをコマンドリストに設定
	Dx12Wrapper::Instance().CommandList()->RSSetViewports(1, Dx12Wrapper::Instance().ViewPort());			
	//シザー矩形をコマンドリストに設定
	Dx12Wrapper::Instance().CommandList()->RSSetScissorRects(1, Dx12Wrapper::Instance().Rect());								
}

/// <summary>
/// ペラポリゴン用ルートシグネチャ・パイプラインをセットする関数
/// </summary>
void
PeraRenderer::SetPeraPipelines()
{
	//パイプラインステート、ルートシグネチャ、プリミティブトポロジーをセット
	Dx12Wrapper::Instance().SetPipelines(																
		_peraRS.Get(), _peraPipeline.Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//ペラポリゴン用SRV、及びハンドルを設定
	ID3D12DescriptorHeap* peraSRVHeaps[] = { _peraSRVHeap.Get() };					
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, peraSRVHeaps);
	auto handle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(0, handle);

	//エフェクト適用データ用CBV、及びハンドルを設定
	ID3D12DescriptorHeap* factorCBVHeap[] = { Dx12Wrapper::Instance().FactorCBVHeap() };				
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, factorCBVHeap);
	handle = Dx12Wrapper::Instance().FactorCBVHeap()->GetGPUDescriptorHandleForHeapStart();
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(1, handle);

	//データを更新
	Dx12Wrapper::Instance().UpdateFade();																
}

/// <summary>
/// ペラポリゴン用リソースの遷移(RENDER_TARGET→SHADER_RESOURCE)を実行する関数
/// </summary>
void
PeraRenderer::EndPeraDraw()
{
	//ペラポリゴン用リソースをSHADER_RESOURCEに遷移
	Dx12Wrapper::Instance().BarrierTransition(							
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
}

/// <summary>
/// ペラポリゴン用頂点バッファービューを返す関数
/// </summary>
/// <returns>ビュー</returns>
D3D12_VERTEX_BUFFER_VIEW*
PeraRenderer::PeraVBView()
{
	return &_peraVBV;
}

/// <summary>
/// ペラポリゴン用パイプラインを返す関数
/// </summary>
/// <returns>パイプラインのポインタ</returns>
ID3D12PipelineState*
PeraRenderer::PeraPipeline()
{
	return _peraPipeline.Get();
}

/// <summary>
/// ペラポリゴン用ルートシグネチャを返す関数
/// </summary>
/// <returns>ルートシグネチャのポインタ</returns>
ID3D12RootSignature*
PeraRenderer::PeraRootSignature()
{
	return _peraRS.Get();
}