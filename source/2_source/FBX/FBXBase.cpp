#include "Functions.h"
#include "FBX/FBXBase.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="filePath">モデル格納ファイル名</param>
/// <param name="size">当たり判定の大きさ</param>
/// <param name="pos">初期座標</param>
/// <param name="diff">当たり判定の差分</param>
FBXBase::FBXBase(const wchar_t* filePath, const XMFLOAT3& size, const XMFLOAT3& pos, const XMFLOAT3& diff)
	:_pos(XMLoadFloat3(&pos))
{
	_collectNormal = true;

	//モデル関連の情報を初期化
	InitModel(filePath);														

	//頂点バッファー・ビュー作成
	CreateVertexBufferView();													
	//インデックスバッファー・ビュー作成
	CreateIndexBufferView();													
	//シェーダーリソース・ビュー作成
	CreateShaderResourceView();													

	//当たり判定を作成
	_collider = make_shared<BoxCollider>(size, diff);
}

/// <summary>
/// デストラクタ
/// ポインタを開放する
/// </summary>
FBXBase::~FBXBase()
{

}

/// <summary>
/// モデル関連の初期化を行う関数
/// </summary>
/// <param name="filePath">モデルのパス</param>
void
FBXBase::InitModel(const wchar_t* filePath)
{
	//モデル読み込み用設定
	ImportSettings settings =										
	{
		//メッシュ情報
		_meshes,				
		//ボーン名とインデックスの連想配列
		_boneMapping,												
		//行列などのボーン情報のベクトル
		_boneInfo,													
		//U・V座標を反転させるか
		false,														
		true,
	};

	//モデル情報が格納されているシーンを読み込む
	auto& loader = AssimpLoader::Instance();
	_scene = loader.LoadScene(filePath);
	//シーンを元にモデルを読み込む
	if (_scene != nullptr)
	{
		loader.LoadModel(_scene, filePath, settings);
	}
	else
	{
		assert(0);
	}
}

/// <summary>
/// メッシュ毎に頂点バッファー・ビューを作成する関数
/// </summary>
/// <returns>処理が成功したかどうか</returns>
HRESULT
FBXBase::CreateVertexBufferView()
{
	//返り値を初期化
	result = S_OK;											

	_normals.emplace_back(_meshes[0].vertices[0]);

	//ビュー数をメッシュ数に合わせる
	_vbViews.reserve(_meshes.size());													
	//全メッシュに対し処理を実行
	for (size_t i = 0; i < _meshes.size(); i++)											
	{
		//格納用バッファー・ビュー
		ID3D12Resource* tmpVertexBuffer = nullptr;										
		D3D12_VERTEX_BUFFER_VIEW tmpVBView = {};

		//頂点全体のデータサイズ
		auto size = sizeof(FBXVertex) * _meshes[i].vertices.size();

		if (_collectNormal)
		{
			for (auto& vert : _meshes[i].vertices)
			{
				_normals.emplace_back(vert);
				auto vec = XMLoadFloat3(&vert.normal);

				for (auto& a : _normals)
				{
					auto aVec = XMLoadFloat3(&a.normal);
					auto b = fabs(1.0f - XMVector3Dot(vec, aVec).m128_f32[0]);
					if (b <= FLT_EPSILON)
					{
						_normals.pop_back();
						break;
					}
				}
			}
		}

		//ヒーププロパティ
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);				
		//リソース設定
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);								

		//バッファー作成
		result = Dx12Wrapper::Instance().Device()->CreateCommittedResource								
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpVertexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//頂点データ取得し、バッファーへ書き込む
		vector<FBXVertex> data = _meshes[i].vertices;
		FBXVertex* mappedVertex = nullptr;
		tmpVertexBuffer->Map(0, nullptr, (void**)&mappedVertex);
		copy(begin(data), end(data), mappedVertex);
		tmpVertexBuffer->Unmap(0, nullptr);

		//ビューにバッファー情報を書き込む
		tmpVBView.BufferLocation = tmpVertexBuffer->GetGPUVirtualAddress();				
		tmpVBView.SizeInBytes = static_cast<UINT>(tmpVertexBuffer->GetDesc().Width);
		tmpVBView.StrideInBytes = sizeof(FBXVertex);

		//ビューをベクトルに格納
		_vbViews.push_back(tmpVBView);
	}

	return result;
}

/// <summary>
/// メッシュ毎にインデックスバッファー・ビューを作成する関数
/// </summary>
/// <returns></returns>
HRESULT
FBXBase::CreateIndexBufferView()
{
	//返り値を初期化
	result = S_OK;

	//インデックスバッファービューを用意する									
	_ibViews.reserve(_meshes.size());

	//全メッシュに対し処理を実行
	for (size_t i = 0; i < _meshes.size(); i++)										
	{
		//格納用バッファー・ビュー
		ID3D12Resource* tmpIndexBuffer = nullptr;									
		D3D12_INDEX_BUFFER_VIEW tmpIBView = {};

		//インデックス全体のデータサイズ
		auto size = sizeof(uint32_t) * _meshes[i].indices.size();					

		//ヒーププロパティ
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);			
		//リソース設定
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);							

		//バッファー作成
		result = Dx12Wrapper::Instance().Device()->CreateCommittedResource							
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpIndexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//インデックスデータを取得・バッファーに書き込む
		vector<uint32_t> data = _meshes[i].indices;
		uint32_t* mappedIndex = nullptr;
		tmpIndexBuffer->Map(0, nullptr, (void**)&mappedIndex);
		copy(begin(data), end(data), mappedIndex);
		tmpIndexBuffer->Unmap(0, nullptr);

		//ビューにバッファー情報を書き込む
		tmpIBView.BufferLocation = tmpIndexBuffer->GetGPUVirtualAddress();			
		tmpIBView.Format = DXGI_FORMAT_R32_UINT;
		tmpIBView.SizeInBytes = static_cast<UINT>(size);

		//インデックスバッファービューを配列に追加								
		_ibViews.push_back(tmpIBView);
	}

	return result;
}

/// <summary>
/// オブジェクトのテクスチャに用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXBase::CreateShaderResourceView()
{
	//返り値を初期化
	result = S_OK;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};												
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = static_cast<UINT>(_meshes.size());
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(_texHeap.ReleaseAndGetAddressOf()));

	//SRV用構造体の作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};												
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//ヒープの先頭アドレス(CPU)
	auto CPUHeapHandle = _texHeap->GetCPUDescriptorHandleForHeapStart();						
	//ヒープの先頭アドレス(GPU)	
	auto GPUHeapHandle = _texHeap->GetGPUDescriptorHandleForHeapStart();					
	//先頭アドレスのずらす幅
	auto incrementSize =	
		Dx12Wrapper::Instance().Device()->GetDescriptorHandleIncrementSize(texHeapDesc.Type);

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

	//各メッシュに対しテクスチャの読み込み処理
	for (size_t i = 0; i < _meshes.size(); i++)													
	{
		ID3D12Resource* tmpTexBuffer = nullptr;

		//テクスチャのパス
		wstring path = _meshes[i].diffuseMap;													
		//テクスチャが無かったら灰色テクスチャを入れる
		if (wcscmp(path.c_str(), L"") == 0)														
		{
			vector<unsigned char> data(4 * 4 * 4);
			fill(data.begin(), data.end(), 0x80);

			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			width = 4;
			height = 4;
			arraySize = 1;
			mipLevels = 1;

			pixels = data.data();
			rowPitch = 4 * 4;
			slicePitch = static_cast<UINT>(data.size());
		}
		//通常テクスチャ
		else																					
		{
			//拡張子を取得
			auto ext = FileExtension(path);														
			//拡張子が"psd"だった場合、"tga"に変換する
			if (wcscmp(ext.c_str(), L"psd") == 0)												
			{
				path = ReplaceExtension(path, "tga");
				ext = FileExtension(path);
			}

			//拡張子に応じて読み込み関数を変える
			result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](path, &meta, scratch);				
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
		}

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
			IID_PPV_ARGS(&tmpTexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//テクスチャの書き込み
		result = tmpTexBuffer->WriteToSubresource(0,											
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
		srvDesc.Format = tmpTexBuffer->GetDesc().Format;										
		Dx12Wrapper::Instance().Device()->CreateShaderResourceView(tmpTexBuffer, &srvDesc, CPUHeapHandle);

		//GPUのアドレスを追加
		_gpuHandles.push_back(GPUHeapHandle);													

		//CPU・GPUのアドレスをずらす
		CPUHeapHandle.ptr += incrementSize;														
		GPUHeapHandle.ptr += incrementSize;
	}

	return result;
}

/// <summary>
/// 毎フレームの描画処理を実行する関数
/// </summary>
void
FBXBase::Draw()
{
	//座標変換用ディスクリプタヒープをセット
	ID3D12DescriptorHeap* transformHeaps[] = { _transHeap.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, transformHeaps);

	//ルートパラメータとディスクリプタヒープのハンドルを関連付け
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(1, _transHeap->GetGPUDescriptorHandleForHeapStart());

	//モデルを構成するメッシュ毎に以下の処理を行う
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		//頂点・インデックスバッファービューのセット
		Dx12Wrapper::Instance().CommandList()->IASetVertexBuffers(0, 1, &_vbViews[i]);
		Dx12Wrapper::Instance().CommandList()->IASetIndexBuffer(&_ibViews[i]);

		//テクスチャバッファービューのセット
		ID3D12DescriptorHeap* SetTexHeap[] = { _texHeap.Get() };
		Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, SetTexHeap);
		Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(2, _gpuHandles[i]);

		//メッシュの描画
		Dx12Wrapper::Instance().CommandList()->DrawIndexedInstanced(static_cast<UINT>(_meshes[i].indices.size()), 1, 0, 0, 0);
	}
}

/// <summary>
/// 当たり判定を返す
/// </summary>
/// <returns>当たり判定</returns>
shared_ptr<BoxCollider>
FBXBase::Collider()const
{
	return _collider;
}