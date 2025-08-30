#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/AssimpLoader.h"
#include "FBX/FbxComposition.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// モデル関連の初期化を行う関数
/// </summary>
/// <param name="filePath">モデルのパス</param>
void
FbxComposition::InitModel(const wchar_t* filePath)
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
FbxComposition::CreateVertexBufferView()
{
	//返り値を初期化
	HRESULT result = S_OK;

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
FbxComposition::CreateIndexBufferView()
{
	//返り値を初期化
	HRESULT result = S_OK;

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
FbxComposition::CreateShaderResourceView()
{
	//返り値を初期化
	HRESULT result = S_OK;

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
/// 矩形の当たり判定を作成する関数
/// </summary>
/// <param name="size">サイズ</param>
/// <param name="pos">初期座標</param>
/// <param name="obj">紐づけるオブジェクト</param>
void 
FbxComposition::CreateBoxCollider(const Vector3& size, const Vector3& pos,IFbx* obj)
{
	_collider = make_shared<BoxCollider>();
	dynamic_pointer_cast<BoxCollider>(_collider)->Init(size, pos, obj);

	_shiftColMatrix = XMMatrixTranslation(
		0, 
		dynamic_pointer_cast<BoxCollider>(_collider)->HalfLength().Y(), 
		0);
}

/// <summary>
/// 球体の当たり判定を作成する関数
/// </summary>
/// <param name="radius">半径</param>
/// <param name="pos">初期座標</param>
/// <param name="obj">紐づけるオブジェクト</param>
void 
FbxComposition::CreateSphereCollider(float radius, const Vector3& pos, IFbx* obj)
{
	_collider = make_shared<SphereCollider>();
	dynamic_pointer_cast<SphereCollider>(_collider)->Init(radius, pos, obj);

	_shiftColMatrix = XMMatrixTranslation(
		0,
		radius / 2,
		0);
}

/// <summary>
/// 毎フレームの描画処理を実行する関数
/// </summary>
void
FbxComposition::Draw()
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
/// 毎フレームの座標変換を行う
/// </summary>
void
FbxComposition::Update()
{
	//当たり判定を上にずらす
	//こう書かないと当たり判定の中心がオブジェクト下になってしまう
	_collider->Update(_shiftColMatrix * _mappedMats[0]);

	if (dynamic_pointer_cast<BoxCollider>(_collider))
	{
		auto tempBoxCol = dynamic_pointer_cast<BoxCollider>(_collider);

		//正面ベクトルを設定
		_frontVec = XMVectorSet(0, tempBoxCol->HalfLength().Y(), tempBoxCol->HalfLength().Z(), 0);
		_frontVec = XMVector3Transform(_frontVec, _mappedMats[0]);

		//表示座標を当たり判定の中心から高さの半分だけずらした箇所にする
		auto yDiff = tempBoxCol->HalfLength().Y();
		_currentPosition = *_collider->Center() - Vector3(0, yDiff, 0);
	}
	else if (dynamic_pointer_cast<SphereCollider>(_collider))
	{
		auto radius = dynamic_pointer_cast<SphereCollider>(_collider)->Radius();
		_currentPosition = *_collider->Center() - Vector3(0,radius / 2,0);
	}
}

/// <summary>
/// 当たり判定を返す
/// </summary>
/// <returns>当たり判定</returns>
shared_ptr<ICollider>
FbxComposition::Collider()const
{
	return _collider;
}

/// <summary>
/// 表示用座標を返す
/// </summary>
/// <returns>表示用座標</returns>
Vector3
FbxComposition::CurrentPosition()const
{
	return _currentPosition;
}

/// <summary>
/// 正面ベクトルを返す
/// </summary>
/// <returns>正面ベクトル</returns>
Vector3
FbxComposition::FrontVec()const
{
	return _frontVec;
}

/// <summary>
/// 足元ベクトルを返す
/// </summary>
/// <returns>足元ベクトル</returns>
Vector3
FbxComposition::FootVec()const
{
	return _footVec;
}

/// <summary>
/// オブジェクトの速度を返す
/// </summary>
/// <returns>速度</returns>
Vector3
FbxComposition::Speed()const
{
	return _speed;
}

/// <summary>
/// オブジェクトの名前を返す
/// </summary>
/// <returns>名前</returns>
const string
FbxComposition::Name()
{
	return _name;
}

/// <summary>
/// ボーン変換を排除するかどうかを決める
/// </summary>
/// <param name="val">ボーン変換を決める真理値</param>
void
FbxComposition::SetRejectBone(bool val)
{
	_rejectBone = val;
}