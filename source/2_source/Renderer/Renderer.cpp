#include "Functions.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

//トゥーンテクスチャ格納パス
const wchar_t* TOON_PATH = L"Asset/model/textures/toon01.bmp";

/// <summary>
/// コンストラクタ
/// </summary>
Renderer::Renderer()
{

}

/// <summary>
/// デストラクタ
/// </summary>
Renderer::~Renderer()
{

}

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
Renderer&
Renderer::Instance()
{
	static Renderer instance;
	return instance;
}

/// <summary>
/// 初期化関数
/// </summary>
void
Renderer::Init()
{
	//ルートシグネチャ初期化
	CreateRootSignature();						
	//パイプラインステート初期化
	CreateGraphicsPipelineForPMD();
	//トゥーンテクスチャ用SRV作成
	CreateToonShaderResourceView();
}

/// <summary>
/// ルートシグネチャを初期化する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Renderer::CreateRootSignature()
{
	//シェーダーのスロットと紐付くディスクリプタレンジの作成
	CD3DX12_DESCRIPTOR_RANGE descTblRange[4] = {};
	//定数[b0]
	descTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);		
	//定数[b1]
	descTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		
	//テクスチャ[t0]
	descTblRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		
	//テクスチャ[t1]
	descTblRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);		

	//バッファーとディスクリプタレンジを紐づけるルートパラメータの作成
	CD3DX12_ROOT_PARAMETER rootparam[4] = {};
	//ビュー・プロジェクション用バッファーを管理
	rootparam[0].InitAsDescriptorTable(1, &descTblRange[0]);			
	//座標変換用バッファー
	rootparam[1].InitAsDescriptorTable(1, &descTblRange[1]);			
	//テクスチャ用バッファー
	rootparam[2].InitAsDescriptorTable(1, &descTblRange[2],				
		D3D12_SHADER_VISIBILITY_PIXEL);
	//トゥーンテクスチャ用バッファー
	rootparam[3].InitAsDescriptorTable(1, &descTblRange[3],				
		D3D12_SHADER_VISIBILITY_PIXEL);

	//サンプラー（テクスチャのXY座標の取得法を決める）の作成
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};						
	//サンプラー[s0]
	samplerDescs[0].Init(0);
	//サンプラー[s1]
	samplerDescs[1].Init(1);
	samplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};					//設定用構造体の作成・設定
	rootSignatureDesc.Init(
		4,
		rootparam,
		2,
		samplerDescs,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ID3DBlob* rootSigBlob = nullptr;									//ルートシグネチャの初期化
	ID3DBlob* errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	result = Dx12Wrapper::Instance().Device()->CreateRootSignature(						//ルートシグネチャの作成
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	rootSigBlob->Release();												//不要になったデータを開放

	return result;
}

/// <summary>
/// グラフィックスパイプラインを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Renderer::CreateGraphicsPipelineForPMD()
{
	ID3DBlob* errorBlob = nullptr;														//エラー判別用オブジェクト

	auto result = S_OK;

	//GPUに頂点データをどう解釈するか教える
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		//座標
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//法線
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//テクスチャUI値
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//正接
		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//マテリアル
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//ボーンID
		{"IDs",0,DXGI_FORMAT_R32G32B32A32_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//ボーン重み
		{"WEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline = {};									//パイプラインステート設定用構造体
	gPipeline.pRootSignature = _rootSignature.Get();									//ルートシグネチャセット

//#ifdef _DEBUG

	ID3DBlob* vsBlob = nullptr;															//頂点シェーダーオブジェクト
	ID3DBlob* psBlob = nullptr;															//ピクセルシェーダーオブジェクト

	result = D3DCompileFromFile(														//頂点シェーダー読み込み
		L"3_shader/FBX/FBXVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"FBXVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (!CheckCompilerResult(result, errorBlob))
	{
		assert(0);
		return result;
	}

	result = D3DCompileFromFile(														//ピクセルシェーダー読み込み
		L"3_shader/FBX/FBXPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"FBXPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (!CheckCompilerResult(result, errorBlob))
	{
		assert(0);
		return result;
	}

	gPipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob);										//頂点シェーダーセット
	gPipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob);										//ピクセルシェーダーセット

//#else
//
//	gPipeline.VS.pShaderBytecode = g_FBXVS;												//頂点シェーダーのバイナリとメモリサイズ
//	gPipeline.VS.BytecodeLength = sizeof(g_FBXVS);
//	gPipeline.PS.pShaderBytecode = g_FBXPS;												//ピクセルシェーダー
//	gPipeline.PS.BytecodeLength = sizeof(g_FBXPS);
//
//#endif

	gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;									//サンプルマスク

	//ブレンドステート
	gPipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gPipeline.BlendState.RenderTarget[0].BlendEnable = true;
	gPipeline.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gPipeline.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	
	gPipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);					//ポリゴンをピクセルに変換する仕組みを設定
	gPipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;							//描画不要なポリゴンを描画しないよう設定

	//深度値周りの設定
	gPipeline.DepthStencilState.DepthEnable = true;										//深度値を有効に
	gPipeline.DepthStencilState.StencilEnable = false;									//ステンシル値は無効のまま
	gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;			//深度バッファーに深度値を書き込むよう設定
	gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;					//ピクセル同士の深度を比較し、小さい方を描画

	gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;										//深度値は32bit値で表現

	gPipeline.InputLayout.pInputElementDescs = inputLayout;								//入力レイアウト設定
	gPipeline.InputLayout.NumElements = _countof(inputLayout);							//レイアウト配列の要素数

	gPipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;			//ポリゴンの切り取り方
	gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;			//ポリゴンを三角形で表現する

	gPipeline.NumRenderTargets = 1;														//レンダーターゲット数
	gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;								//レンダーターゲットのフォーマット

	gPipeline.SampleDesc.Count = 1;														//ピクセル当たりサンプリング数
	gPipeline.SampleDesc.Quality = 0;													//クオリティ

	result = Dx12Wrapper::Instance().Device()->CreateGraphicsPipelineState(&gPipeline,
		IID_PPV_ARGS(_pipeline.ReleaseAndGetAddressOf()));								//パイプラインステート作成

	return result;
}

/// <summary>
/// トゥーンテクスチャ用SRVを作成する関数
/// </summary>
/// <returns>関数が成功したかどうか</returns>
HRESULT
Renderer::CreateToonShaderResourceView()
{
	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = 1;
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	auto result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(_toonHeap.ReleaseAndGetAddressOf()));

	//SRV用構造体の作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープの先頭アドレス(CPU)
	auto CPUHeapHandle = _toonHeap->GetCPUDescriptorHandleForHeapStart();
	//ヒープの先頭アドレス(GPU)	
	_GpuToonHeapHandle = _toonHeap->GetGPUDescriptorHandleForHeapStart();

	//テクスチャ読み込み用データ
	TexMetadata meta = {};
	ScratchImage scratch = {};
	DXGI_FORMAT format;
	size_t width;
	size_t height;
	UINT16 arraySize;
	UINT16 mipLevels;
	void* pixels;
	UINT rowPitch;
	UINT slicePitch;

	//トゥーンテクスチャ用バッファー
	ID3D12Resource* toonBuffer = nullptr;

	//拡張子を取得
	auto ext = FileExtension(TOON_PATH);

	//拡張子に応じて読み込み関数を変える
	result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](TOON_PATH, &meta, scratch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//テクスチャデータの用意
	auto img = scratch.GetImage(0, 0, 0);
	format = meta.format;
	width = meta.width;
	height = meta.height;
	arraySize = static_cast<UINT16>(meta.arraySize);
	mipLevels = static_cast<UINT16>(meta.mipLevels);
	pixels = img->pixels;
	rowPitch = static_cast<UINT>(img->rowPitch);
	slicePitch = static_cast<UINT>(img->slicePitch);

	//バッファー用ヒーププロパティ
	auto heapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	//リソース設定
	auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
		format,
		width,
		height,
		arraySize,
		mipLevels);

	//バッファーの作成
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource
	(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&toonBuffer)
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//テクスチャの書き込み
	result = toonBuffer->WriteToSubresource(0,
		nullptr,
		pixels,
		rowPitch,
		slicePitch
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//ビューのフォーマットをテクスチャに合わせ、作成
	srvDesc.Format = toonBuffer->GetDesc().Format;
	Dx12Wrapper::Instance().Device()->CreateShaderResourceView(toonBuffer, &srvDesc, CPUHeapHandle);

	return result;
}

/// <summary>
/// シェーダー読み込みが成功したかどうかを判別する関数
/// </summary>
/// <param name="result">シェーダー読み込み関数の返り値</param>
/// <param name="error">エラーデータ格納用オブジェクト</param>
/// <returns>成功したかどうか</returns>
bool
Renderer::CheckCompilerResult(HRESULT result, ID3DBlob* error)
{
	if (FAILED(result))												//関数が失敗したら
	{
		
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))		//ファイルが見つからない場合その旨を記述
		{
			::OutputDebugStringA("ファイルが見つかりません");
		}
		else
		{
			string errStr;											//エラー情報記述用文字列
			errStr.resize(error->GetBufferSize());					//文字列のサイズを変更

			copy_n(													//オブジェクトのアドレスからエラー原因を取得し、文字列にコピー
				(char*)error->GetBufferPointer(),
				error->GetBufferSize(),
				errStr.begin());

			errStr += "\n";											//末尾に改行文字を加える
			OutputDebugStringA(errStr.c_str());						//出力
		}
		return false;												//関数が失敗したら偽を返す
	}
	else
	{
		return true;												//関数が成功したら真を返す
	}
}

/// <summary>
/// 描画用関数
/// </summary>
void
Renderer::Draw()
{
	//SRVのセット
	ID3D12DescriptorHeap* SetToonHeap[] = { _toonHeap.Get() };
	//トゥーンテクスチャのデータをGPUに渡す
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, SetToonHeap);
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(3, _GpuToonHeapHandle);
}

/// <summary>
///	パイプラインステートを返す関数
/// </summary>
/// <returns>パイプラインステート</returns>
ID3D12PipelineState*
Renderer::GetPipelineState()const
{
	return _pipeline.Get();
}

/// <summary>
/// ルートシグネチャを返す関数
/// </summary>
/// <returns>ルートシグネチャ</returns>
ID3D12RootSignature*
Renderer::GetRootSignature()const
{
	return _rootSignature.Get();
}