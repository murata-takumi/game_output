#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/BoxCollider2D.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/FbxActor.h"
#include "Manager/InputManager.h"
#include "Manager/SpriteManager.h"
#include "Package/Package.h"
#include "Wrapper/Dx12Wrapper.h"

const int LOADING_WIDTH = 480;		//ロード画面で表示するアイコンの幅
const int LOADING_HEIGHT = 270;		//ロード画面で表示するアイコンの高さ

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
SpriteManager&
SpriteManager::Instance()
{
	static SpriteManager instance;
	return instance;
}

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="width">ウィンドウ幅</param>
/// <param name="height">ウィンドウ高さ</param>
void
SpriteManager::Init(LONG width, LONG height)
{
	_width = width;
	_height = height;

	//Sprite用ルートシグネチャを作成
	CreateSpriteRS();

	//Sprite用オブジェクトを初期化
	InitSpriteDevices();

	_startColl = make_unique<BoxCollider2D>(XMVectorSet(240, 60, 0, 0), XMVectorSet(610, 535, 0, 0));
	_exitColl = make_unique<BoxCollider2D>(XMVectorSet(240, 60, 0, 0), XMVectorSet(610, 635, 0, 0));

	//ロード画面用矩形を調整
	AdjustSpriteRect();

	//ヒープハンドルの差分のサイズを取得
	_incrementSize =
		Dx12Wrapper::Instance().Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//ロード画面に回転するアイコン
	CreateUIBufferView(L"Asset/image/loading/1.png", "load_1");
	CreateUIBufferView(L"Asset/image/loading/2.png", "load_2");
	CreateUIBufferView(L"Asset/image/loading/3.png", "load_3");
	CreateUIBufferView(L"Asset/image/loading/4.png", "load_4");
	CreateUIBufferView(L"Asset/image/loading/5.png", "load_5");
	CreateUIBufferView(L"Asset/image/loading/6.png", "load_6");
	CreateUIBufferView(L"Asset/image/loading/7.png", "load_7");
	CreateUIBufferView(L"Asset/image/loading/8.png", "load_8");

	CreateUIBufferView(L"Asset/image/button.png", "start");
	CreateUIBufferView(L"Asset/image/button.png", "exit");

	CreateUIBufferView(L"Asset/image/background.png", "background");
}

/// <summary>
/// SpriteBatch向けのルートシグネチャ・シェーダーを作成する関数
/// </summary>
/// <returns>処理が成功したかどうか</returns>
HRESULT
SpriteManager::CreateSpriteRS()
{
	//ディスクリプタレンジ(SRV用)
	CD3DX12_DESCRIPTOR_RANGE spriteTblRange[2] = {};					
	spriteTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	spriteTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	
	//ルートパラメータ(SRV,CBV用)
	CD3DX12_ROOT_PARAMETER spriteRootParam[3] = {};						
	spriteRootParam[0].InitAsDescriptorTable(1, &spriteTblRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootParam[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	spriteRootParam[2].InitAsDescriptorTable(1, &spriteTblRange[1], D3D12_SHADER_VISIBILITY_ALL);

	//サンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};						
	samplerDesc.Init(0);
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//ルートシグネチャの初期化
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};							
	rsDesc.Init(3,spriteRootParam,1,&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ID3DBlob* rsBlob = nullptr;											
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
	//使わないデータを開放
	rsBlob->Release();							

	//ルートシグネチャ作成
	result = Dx12Wrapper::Instance().Device()->CreateRootSignature(						
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
	//グラフィックスメモリの初期化
	_gmemory = make_unique<GraphicsMemory>(Dx12Wrapper::Instance().Device());								

	//スプライト表示用オブジェクトの初期化
	ResourceUploadBatch resUploadBatch(Dx12Wrapper::Instance().Device());									
	resUploadBatch.Begin();

	//レンダーターゲットステート
	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);		

	//サンプラーを取得するためStateオブジェクトを初期化
	unique_ptr<CommonStates> _states = make_unique<CommonStates>(Dx12Wrapper::Instance().Device());		

	//GPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE wrap = _states->AnisotropicWrap();						

	SpriteBatchPipelineStateDescription psd(rtState, &CommonStates::NonPremultiplied);

	//スプライト表示用オブジェクト
	_spriteBatch = make_unique<SpriteBatch>(Dx12Wrapper::Instance().Device(), resUploadBatch, psd);		

	_batch = make_unique<PrimitiveBatch<VertexPositionColor>>(Dx12Wrapper::Instance().Device());

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_WIREFRAME,
		D3D12_CULL_MODE_BACK,false,D3D12_DEFAULT_DEPTH_BIAS,D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
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

	_effect = make_unique<BasicEffect>(Dx12Wrapper::Instance().Device(), EffectFlags::VertexColor, ePsd);

	_world = SimpleMath::Matrix::Identity;
	_view = Dx12Wrapper::Instance().ViewMatrix();
	_proj = Dx12Wrapper::Instance().ProjMatrix();

	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//フォント表示用オブジェクトの初期化		
	Dx12Wrapper::Instance().CreateDescriptorHeap(_heapForSpriteFont,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,256, 
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	_tmpCPUHandle = _heapForSpriteFont->GetCPUDescriptorHandleForHeapStart();
	_tmpGPUHandle = _heapForSpriteFont->GetGPUDescriptorHandleForHeapStart();

	_spriteFont = make_unique<SpriteFont>(Dx12Wrapper::Instance().Device(),
		resUploadBatch,
		L"Asset/font/fonttest.spritefont",
		_tmpCPUHandle,
		_tmpGPUHandle);

	//描画命令をGPU側へ転送
	auto future = resUploadBatch.End(Dx12Wrapper::Instance().CommandQueue());								

	//GPUが使用可能になるまで待機
	Dx12Wrapper::Instance().WaitForCommandQueue();														
	future.wait();

	//スプライト表示用オブジェクトをビューポートへ登録
	_spriteBatch->SetViewport(*Dx12Wrapper::Instance().ViewPort());										

	return;
}

/// <summary>
/// アクターの当たり判定を描画する関数
/// </summary>
/// <param name="collider">当たり判定のポインタ</param>
void
SpriteManager::ColliderDraw(const shared_ptr<ICollider> collider)
{
	//ヒープをコマンドリストにセット
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	auto transMat = XMMatrixTranslationFromVector(
		RightPosToLeftPos(Vector3(
			collider->Center()->X(),
			collider->Center()->Y(),
			collider->Center()->Z())));

	//コマンドリストにグリッドを登録
	_effect->SetWorld(transMat);
	_effect->Apply(Dx12Wrapper::Instance().CommandList());

	if (dynamic_pointer_cast<BoxCollider>(collider))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(collider);
		auto primitive = GeometricPrimitive::CreateBox(
		Vector3(
			tempBox->HalfLength().X() * 2,
			tempBox->HalfLength().Y() * 2,
			tempBox->HalfLength().Z() * 2));
		primitive->Draw(Dx12Wrapper::Instance().CommandList());
	}
	else if (dynamic_pointer_cast<SphereCollider>(collider))
	{
		auto radius = dynamic_pointer_cast<SphereCollider>(collider)->Radius();
		auto primitive = GeometricPrimitive::CreateSphere(radius, 8);
		primitive->Draw(Dx12Wrapper::Instance().CommandList());
	}

	//元に戻す
	_effect->SetWorld(_world);
	_effect->Apply(Dx12Wrapper::Instance().CommandList());
}

/// <summary>
/// マウスカーソルが矩形内にあるかどうか確認する
/// </summary>
/// <param name="rect">対象の矩形</param>
/// <returns>矩形内にあるかどうか</returns>
bool
SpriteManager::IsCursorInRect(RECT rect)
{
	auto cursorPos = InputManager::Instance().MousePos();

	if (cursorPos._posX < rect.left)return false;
	if (rect.right < cursorPos._posX)return false;
	if (cursorPos._posY < rect.top)return false;
	if (rect.bottom < cursorPos._posY)return false;

	return true;
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
	//UI画像読み込み用データ
	TexMetadata meta = {};															
	ScratchImage scratch = {};

	//画像データの読み込み
	auto ext = FileExtension(path);			
	auto result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](path, &meta, scratch);		
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//データを取得
	auto img = scratch.GetImage(0, 0, 0);	
	DXGI_FORMAT format = meta.format;
	size_t width = meta.width;
	size_t height = meta.height;
	UINT16 arraySize = static_cast<UINT16>(meta.arraySize);
	UINT16 mipLevels = static_cast<UINT16>(meta.mipLevels);		
	void* pixels = img->pixels;
	UINT rowPitch = static_cast<UINT>(img->rowPitch);
	UINT slicePitch = static_cast<UINT>(img->slicePitch);

	//画像データ書き込み用バッファ
	ID3D12Resource* tmpUIBuff = nullptr;											

	//リソース作成
	auto uiResDesc = CD3DX12_RESOURCE_DESC::Tex2D(									
		format,
		(UINT)width,
		(UINT)height,
		arraySize,
		(UINT)mipLevels);
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource(								
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

	//画像情報を書き込み
	result = tmpUIBuff->WriteToSubresource(0,										
		nullptr,
		pixels,
		rowPitch,
		slicePitch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	//CPU･GPUハンドルをズラす
	_tmpCPUHandle.ptr += _incrementSize;											
	_tmpGPUHandle.ptr += _incrementSize;

	//UIビューの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};									
	srvDesc.Format = tmpUIBuff->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Dx12Wrapper::Instance().Device()->CreateShaderResourceView(tmpUIBuff, &srvDesc, _tmpCPUHandle);

	//GPUハンドルを連想配列に格納
	_GPUHandles[key] = _tmpGPUHandle;												

	return result;
}

/// <summary>
/// 右手座標を左手座標に変換する関数
/// </summary>
/// <param name="rightPos">右手座標</param>
/// <returns>左手座標</returns>
XMVECTOR
SpriteManager::RightPosToLeftPos(const Vector3& rightPos)
{
	return XMVectorSet(rightPos.X() * -1, rightPos.Y(), rightPos.Z(), 0.0f);
}

/// <summary>
/// 画面サイズ変更時、矩形を調整する関数
/// </summary>
void
SpriteManager::AdjustSpriteRect()
{
	//ロード画面の設定
	_loadingRect = { LOADING_WIDTH,LOADING_HEIGHT,_width - LOADING_WIDTH,_height - LOADING_HEIGHT };
	//背景用矩形の設定
	_BGRect = *Dx12Wrapper::Instance().Rect();

	auto startRect = _startColl->Rect();
	auto exitRect = _exitColl->Rect();

	AdjustWindowRect(&_loadingRect, WS_OVERLAPPEDWINDOW, false);		
	AdjustWindowRect(&startRect, WS_OVERLAPPEDWINDOW, false);
	AdjustWindowRect(&exitRect, WS_OVERLAPPEDWINDOW, false);
	AdjustWindowRect(&_BGRect, WS_OVERLAPPEDWINDOW, false);		
}

/// <summary>
/// ローディング画面での画像を描画する関数
/// </summary>
void
SpriteManager::LoadingDraw()
{
	//ヒープをコマンドリストにセット
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };					
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//バッチをセット
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());										

	//現在の時間に応じて表示する画像を変え、ロードアイコンのアニメーションを表現
	int count = ((clock() / static_cast<int>(60 * 1.4)) % 8) + 1;					
	_spriteBatch->Draw(_GPUHandles["load_" + to_string(count)], XMUINT2(1, 1),
		_loadingRect, Colors::White);

	//バッチを解除
	_spriteBatch->End();															
}

/// <summary>
/// タイトル画面でのボタンの描画
/// </summary>
void
SpriteManager::ButtonDraw()
{
	//ヒープをコマンドリストにセット
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//バッチをセット
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());

	//背景用画像ビューをセット
	_spriteBatch->Draw(_GPUHandles["start"], XMUINT2(1, 1),
		_startColl->Rect(), (TitleIsOnStart() ? Colors::White : Colors::LightGray));

	//背景用画像ビューをセット
	_spriteBatch->Draw(_GPUHandles["exit"], XMUINT2(1, 1),
		_exitColl->Rect(), (TitleIsOnExit() ? Colors::White : Colors::LightGray));

	string start = "Start";
	string exit = "Exit";

	_spriteFont->DrawString(_spriteBatch.get(),start.c_str(),_startColl->StrCenterPos(start), DirectX::Colors::GhostWhite);
	_spriteFont->DrawString(_spriteBatch.get(), exit.c_str(),_exitColl->StrCenterPos(exit), DirectX::Colors::GhostWhite);

	//バッチを解除
	_spriteBatch->End();
}

/// <summary>
/// 背景を描画する関数
/// </summary>
void
SpriteManager::BackGroundDraw()
{
	//ヒープをコマンドリストにセット
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//バッチをセット
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());

	//背景用画像ビューをセット
	_spriteBatch->Draw(_GPUHandles["background"], XMUINT2(1, 1),
		_BGRect, Colors::White);

	//バッチを解除
	_spriteBatch->End();
}

/// <summary>
/// グリッドを描画する関数
/// </summary>
void
SpriteManager::GridDraw(const map<string, shared_ptr<IFbx>> actorAndObjs)
{
	//ビュー･プロジェクション行列を取得
	_view = Dx12Wrapper::Instance().ViewMatrix();											
	_proj = Dx12Wrapper::Instance().ProjMatrix();

	//ワールド・ビュー・プロジェクション行列をセット
	_effect->SetWorld(_world);											
	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//コマンドリストにグリッドを登録
	_effect->Apply(Dx12Wrapper::Instance().CommandList());								

	//描画開始
	_batch->Begin(Dx12Wrapper::Instance().CommandList());

	//X軸のグリッドの描画
	VertexPositionColor v1(_origin - _XAxis, { 0.75f,0.0f,0.0f,.10f });		
	VertexPositionColor v2(_origin + _XAxis, { 0.75f,0.0f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	//Y軸のグリッドの描画
	v1 = VertexPositionColor(_origin - _YAxis, { 0.f,0.75f,0.0f,.10f });	
	v2 = VertexPositionColor(_origin + _YAxis, { 0.f,0.75f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	//X軸のグリッドの描画
	v1 = VertexPositionColor(_origin - _ZAxis, { 0.0f,0.0f,0.75f,.10f });	
	v2 = VertexPositionColor(_origin + _ZAxis, { 0.0f,0.0f,0.75f,.10f });
	_batch->DrawLine(v1, v2);

	//当たり判定を持つオブジェクトが存在したら当たり判定の描画
	for (const auto& actor : actorAndObjs)
	{
		if (actor.second != nullptr)
		{
			if (dynamic_pointer_cast<FbxActor>(actor.second))
			{
				ColliderDraw(dynamic_pointer_cast<FbxActor>(actor.second)->GetColForGround());
			}

			ColliderDraw(actor.second->Collider());
			//X軸のグリッドの描画
			v1 = VertexPositionColor(RightPosToLeftPos(actor.second->CurrentPosition()), {1.0f,0.0f,0.0f,.10f});
			v2 = VertexPositionColor(RightPosToLeftPos(*actor.second->Collider()->Center()), {1.0f,0.0f,0.0f,.10f});
			_batch->DrawLine(v1, v2);
		}
	}

	//描画終了
	_batch->End();
}

/// <summary>
/// グラフィックスメモリをコマンドキューにセットする関数
/// </summary>
void
SpriteManager::Commit()
{
	_gmemory->Commit(Dx12Wrapper::Instance().CommandQueue());
}

/// <summary>
/// スタートボタンが押されているか判別
/// </summary>
/// <returns>スタートボタン上にカーソルがあるか</returns>
bool
SpriteManager::TitleIsOnStart()
{
	return IsCursorInRect(_startColl->Rect());
}

/// <summary>
/// 終了ボタンが押されているか判別
/// </summary>
/// <returns>終了ボタン上にカーソルがあるか</returns>
bool
SpriteManager::TitleIsOnExit()
{
	return IsCursorInRect(_exitColl->Rect());
}