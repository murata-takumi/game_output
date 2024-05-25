#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/CommonStates.h>
#include <DirectXTK12-master/Inc/Effects.h>
#include <DirectXTK12-master/Inc/PrimitiveBatch.h>
#include <DirectXTK12-master/Inc/ResourceUploadBatch.h>
#include <DirectXTK12-master/Inc/SpriteBatch.h>
#include <DirectXTK12-master/Inc/SpriteFont.h>
#include <DirectXTK12-master/Inc/VertexTypes.h>

#include <sstream>
#include <time.h>

/// <summary>
/// 画像や文字フォントを管理するクラス
/// </summary>
class BoxCollider;
class Dx12Wrapper;
class FBXBase;
class FBXActor;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	Dx12Wrapper& _dx12;														//Dx12Wrapperインスタンス

	unique_ptr<GraphicsMemory> _gmemory;									//グラフィックスメモリ
	unique_ptr<SpriteBatch> _spriteBatch;									//スプライト（画像）表示用オブジェクト

	vector<shared_ptr<FBXBase>> _actorAndObjs;								//FBXBase及び派生オブジェクトのベクトル

	ID3DBlob* _psBlob = nullptr;											//ピクセルシェーダー用データ

	SIZE _winSize;															//ウィンドウサイズ
	LONG _width;															//画面の幅
	LONG _height;															//画面の高さ

	unique_ptr<BasicEffect> _effect;

	unique_ptr<PrimitiveBatch<VertexPositionColor>> _batch;

	SimpleMath::Matrix _world;												//ワールド行列
	SimpleMath::Matrix _view;												//ビュー行列
	SimpleMath::Matrix _proj;												//プロジェクション行列

	ComPtr<ID3D12DescriptorHeap> _heapForSpriteFont = nullptr;				//フォント・画像用ヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								//ヒープハンドル(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								//ヒープハンドル(GPU)
	UINT _incrementSize;													//ハンドルのアドレスの差分

	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					//ハンドル(GPU)のベクトル

	RECT _loadingRect;														//ロード中アイコン用矩形

	ComPtr<ID3D12RootSignature> _spriteRS;									//スプライト描画用ルートシグネチャ

	SimpleMath::Vector3 _XAxis = SimpleMath::Vector3(1000.f, 0.f, 0.0f);	//X軸のグリッドの幅
	SimpleMath::Vector3 _YAxis = SimpleMath::Vector3(0.0f, 1000.f, 0.0f);	//Y軸のグリッドの幅
	SimpleMath::Vector3 _ZAxis = SimpleMath::Vector3(0.0f, 0.0f, 1000.f);	//Z軸のグリッドの幅
	SimpleMath::Vector3 _origin = SimpleMath::Vector3::Zero;				//

	HRESULT CreateSpriteRS();												//SpriteBatch用ルートシグネチャを作成する関数
	HRESULT CreateUIBufferView(const wchar_t* path, string key);			//UI用の画像のバッファー・ビューを作成する関数

	void InitSpriteDevices();												//スプライト・文字列表示用オブジェクトを初期化する関数

	void ColliderDraw(const shared_ptr<BoxCollider> collider);
public:
	SpriteManager(Dx12Wrapper& dx12,LONG width,LONG height);		//コンストラクタ

	void AdjustSpriteRect();										//画面サイズの変更を感知して矩形を調整する

	void LoadingDraw();												//ロード画面での描画
	void GridDraw();												//マウスカーソルを描画

	void Commit();													//グラフィックスメモリをコマンドリストにセット

	void SetActors(vector<shared_ptr<FBXBase>> actorAndObjects);	//アクターを設定する関数
};