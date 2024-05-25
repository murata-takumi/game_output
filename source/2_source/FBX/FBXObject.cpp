#include "Functions.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// 座標変換用バッファー・ビューを作成
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="filePath">モデルファイルのパス</param>
FBXObject::FBXObject(Dx12Wrapper& dx12, const wchar_t* filePath, XMFLOAT3 pos)
	:FBXBase(dx12, filePath, 
		XMFLOAT3(2000.0f, 1.0f, 2000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)
		,pos)
{
	CreateTransformView();											//座標変換用バッファー・ビュー作成
}

/// <summary>
/// デストラクタ
/// 特に何もしない
/// </summary>
FBXObject::~FBXObject()
{

}

/// <summary>
/// オブジェクトの座標変換に用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXObject::CreateTransformView()
{
	result = S_OK;

	auto buffSize = sizeof(XMMATRIX);												//ワールド行列用バッファーの作成
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = _dx12.Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _transBuffer->Map(0, nullptr, (void**)&_mappedMats);					//座標変換用行列の書き込み
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMats[0] = XMMatrixIdentity();
	_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);			//平行移動

	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							//ディスクリプタヒープ設定用構造体の作成
	transformDescHeapDesc.NumDescriptors = 1;										//とりあえずワールドひとつ
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;		//シェーダーから見えるようにする
	transformDescHeapDesc.NodeMask = 0;												//ノードマスクは0で
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;			//デスクリプタヒープ種別

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,			//ヒープの作成
		IID_PPV_ARGS(_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									//ビュー設定用構造体の作成
	cbvDesc.BufferLocation = _transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_transBuffer->GetDesc().Width);

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,								//ビューの作成
		_transHeap->GetCPUDescriptorHandleForHeapStart());

	_collider->Update(_mappedMats[0]);											//当たり判定を初期化

	return result;
}

/// <summary>
/// 毎フレームの座標変換処理を実行する関数
/// </summary>
void
FBXObject::Update()
{

}