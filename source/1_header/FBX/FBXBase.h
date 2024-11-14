#pragma once
#include "Application.h"
#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"

/// <summary>
/// FBXモデルの派生元となるクラス
/// </summary>
class Dx12Wrapper;
class AssimpLoader;
class ImGuiManager;
class FBXBase
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;

protected:
	//関数の返り値
	HRESULT result;							

	//モデルデータを保持するポインタ
	const aiScene* _scene;													

	//当たり判定のポインタ
	shared_ptr<BoxCollider> _collider;										

	//モデル読み込み用メッシュ配列
	vector<Mesh> _meshes;												
	//ボーン名とインデックスの連想配列
	map<string, unsigned int> _boneMapping;							
	//ボーン情報のベクトル
	vector<BoneInfo> _boneInfo;												

	//メッシュ用頂点バッファービュー配列
	vector<D3D12_VERTEX_BUFFER_VIEW> _vbViews;

	//メッシュ用インデックスバッファービュー配列
	vector<D3D12_INDEX_BUFFER_VIEW> _ibViews;								

	//ワールド行列用バッファー
	ComPtr<ID3D12Resource> _transBuffer;								
	//ワールド行列用ヒープ
	ComPtr<ID3D12DescriptorHeap> _transHeap;								

	//テクスチャ用ヒープ
	ComPtr<ID3D12DescriptorHeap> _texHeap;
	//テクスチャバッファービューのGPUハンドル配列
	vector<D3D12_GPU_DESCRIPTOR_HANDLE> _gpuHandles;						

	//モデルの座標変換行列を格納する構造体のポインタ
	XMMATRIX* _mappedMats = nullptr;										

	//座標
	XMVECTOR _pos;

	vector<FBXVertex> _normals;

	//法線を収集するかどうか
	bool _collectNormal = false;

	//モデルを初期化する関数
	void InitModel(const wchar_t* filePath);								

	//頂点バッファー・ビュー作成関数
	HRESULT CreateVertexBufferView();					
	//インデックスバッファー・ビュー作成関数
	HRESULT CreateIndexBufferView();						
	//シェーダーリソース・ビュー作成関数
	HRESULT CreateShaderResourceView();										
	//座標変換用バッファー・ビュー作成関数（継承先で使用）
	virtual HRESULT CreateTransformView() = 0;								

public:
	//コンストラクタ
	FBXBase(const wchar_t* filePath,const XMFLOAT3& size,const XMFLOAT3& pos,
		const XMFLOAT3& diff = XMFLOAT3(0.0f,0.0f,0.0f));
	//デストラクタ
	virtual ~FBXBase();													

	//毎フレームの描画処理
	void Draw();														
	//毎フレームの座標変換処理（継承先で使用）
	virtual void Update() = 0;											

	//当たり判定のポインタを返す関数
	shared_ptr<BoxCollider>Collider()const;								
};