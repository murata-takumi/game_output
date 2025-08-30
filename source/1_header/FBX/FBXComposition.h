#pragma once
#include "Application.h"

class aiScene;
class BoneInfo;
class BoxCollider;
class ICollider;
class SphereCollider;
class Mesh;
class Vector3;
class FbxComposition
{
public:
	//モデルデータを保持するポインタ
	const aiScene* _scene;

	//当たり判定のポインタ
	shared_ptr<ICollider> _collider;

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

	//コンストラクタ、デストラクタ
	FbxComposition() = default;
	~FbxComposition() = default;

	//モデルを初期化する関数
	void InitModel(const wchar_t* filePath);

	//頂点バッファー・ビュー作成関数
	HRESULT CreateVertexBufferView();
	//インデックスバッファー・ビュー作成関数
	HRESULT CreateIndexBufferView();
	//シェーダーリソース・ビュー作成関数
	HRESULT CreateShaderResourceView();

	//当たり判定（矩形）作成関数
	void CreateBoxCollider(const Vector3& size, const Vector3& pos,IFbx* obj);

	//当たり判定（球）作成関数
	void CreateSphereCollider(float radius, const Vector3& pos, IFbx* obj);

	//毎フレームの描画処理
	void Draw();

	//毎フレームの座標変換処理
	void Update();

	//当たり判定のポインタを返す関数
	shared_ptr<ICollider>Collider()const;

	//表示用座標を取得
	Vector3 CurrentPosition()const;

	//正面座標を取得
	Vector3 FrontVec()const;

	//足元座標を取得
	Vector3 FootVec()const;

	//現在の速度
	Vector3 Speed()const;

	//オブジェクト名
	const string Name();

	//ボーン変換を排除するか決める
	void SetRejectBone(bool val);

private:
	
};