#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "FBX/AssimpLoader.h"
#include "FBX/FBXComposition.h"
#include "FBX/FBXObject.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="filePath">fbxファイルのパス</param>
/// <param name="name">オブジェクト名</param>
/// <param name="size">当たり判定のサイズ</param>
/// <param name="pos">当たり判定の座標</param>
/// <returns>処理が成功したかどうか</returns>
HRESULT
FBXObject::Init(const wchar_t* filePath, const string name,
	const Vector3& size, const Vector3& pos)
{
	//共通処理を初期化
	_fbxComp = make_shared<FBXComposition>();

	//モデル関連の情報を初期化
	_fbxComp->InitModel(filePath);

	//頂点バッファー・ビュー作成
	_fbxComp->CreateVertexBufferView();
	//インデックスバッファー・ビュー作成
	_fbxComp->CreateIndexBufferView();
	//シェーダーリソース・ビュー作成
	_fbxComp->CreateShaderResourceView();


	//当たり判定を作成
	_fbxComp->CreateCollider(size, Vector3(0, 0, 0), this);

	//座標変換用バッファー・ビュー作成
	CreateTransformView();

	return S_OK;
}

/// <summary>
/// オブジェクトの座標変換に用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXObject::CreateTransformView()
{
	HRESULT result = S_OK;

	//ワールド行列用バッファーの作成・書き込み
	auto buffSize = sizeof(XMMATRIX);												
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_fbxComp->_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	result = _fbxComp->_transBuffer->Map(0, nullptr, (void**)&(_fbxComp->_mappedMats));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_fbxComp->_mappedMats[0] = XMMatrixIdentity();
	//平行移動
	_fbxComp->_mappedMats[0] *= XMMatrixTranslationFromVector(_fbxComp->_translateVector);

	_fbxComp->Collider()->Update(_fbxComp->_mappedMats[0]);

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(_fbxComp->_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//ビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									
	cbvDesc.BufferLocation = _fbxComp->_transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_fbxComp->_transBuffer->GetDesc().Width);

	Dx12Wrapper::Instance().Device()->CreateConstantBufferView(&cbvDesc, _fbxComp->_transHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// 当たり判定を取得
/// </summary>
/// <returns>スマートポインタ</returns>
shared_ptr<BoxCollider> 
FBXObject::Collider()
{
	return _fbxComp->Collider();
}

/// <summary>
/// 描画処理
/// </summary>
void 
FBXObject::Draw()
{
	_fbxComp->Draw();
}

/// <summary>
/// 更新処理
/// </summary>
void 
FBXObject::Update()
{
	_fbxComp->Update();
}

/// <summary>
/// 現在の座標を取得する関数
/// </summary>
/// <returns>座標</returns>
Vector3
FBXObject::CurrentPosition()
{
	return _fbxComp->CurrentPosition();
}