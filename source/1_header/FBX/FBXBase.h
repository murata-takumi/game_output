#pragma once
#include "Application.h"

/// <summary>
/// FBXモデルの派生元となるクラス
/// </summary>
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class ImGuiManager;
class Vector3;
class aiScene;
class Mesh;
class BoneInfo;
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

	//当たり判定をオブジェクトの半分ほど上にずらすための行列
	XMMATRIX _shiftColMatrix;

	//移動量を格納するベクトル
	Vector3 _translateVector;

	//ImGuiでの表示に使う座標
	Vector3 _currentPosition;

	//正面判定に使用
	Vector3 _frontVec;
	
	//ジャンプ後のアニメーション遷移に使用
	Vector3 _footVec;

	//オブジェクトの速度
	Vector3 _speed;

	//オブジェクトの名前
	const string _name;

	//当たり判定の更新からボーンの座標変換を排除するか
	bool _rejectBone;

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
	FBXBase(const wchar_t* filePath,const string name,const Vector3& size,const Vector3& pos);
	//デストラクタ
	virtual ~FBXBase();													

	//毎フレームの描画処理
	void Draw();	

	//毎フレームの座標変換処理
	virtual void Update();											

	//当たり判定のポインタを返す関数
	shared_ptr<BoxCollider>Collider()const;

	//表示用座標を取得
	Vector3 CurrentPosition()const;

	Vector3 FrontVec()const;

	Vector3 FootVec()const;
	
	Vector3 Speed()const;

	const string Name();

	//ボーン変換を排除するか決める
	void SetRejectBone(bool val);
};