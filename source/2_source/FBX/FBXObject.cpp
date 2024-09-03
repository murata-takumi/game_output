#include "Functions.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

//当たり判定のサイズ
const XMFLOAT3 COL_SIZE = XMFLOAT3(2000.0f, 1.0f, 2000.0f);

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="filePath">モデルパス</param>
/// <param name="pos">初期座標</param>
FBXObject::FBXObject(Dx12Wrapper& dx12, const wchar_t* filePath, XMFLOAT3 pos)
	:FBXBase(dx12, filePath,COL_SIZE, pos)
{
	//座標変換用バッファー・ビュー作成
	CreateTransformView();											
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

	//ワールド行列用バッファーの作成・書き込み
	auto buffSize = sizeof(XMMATRIX);												
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
	result = _transBuffer->Map(0, nullptr, (void**)&_mappedMats);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMats[0] = XMMatrixIdentity();
	//平行移動
	_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);		

	_collider->Update(_mappedMats[0]);

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//ビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									
	cbvDesc.BufferLocation = _transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_transBuffer->GetDesc().Width);

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,_transHeap->GetCPUDescriptorHandleForHeapStart());							

	return result;
}

/// <summary>
/// 毎フレームの座標変換処理を実行する関数
/// </summary>
void
FBXObject::Update()
{
	//TODO:ここに移動処理等を実装したい
}