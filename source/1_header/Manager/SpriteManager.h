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
class BoxCollider2D;
class Dx12Wrapper;
class IFBX;
class FBXActor;
class Vector3;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:	
	//グラフィックスメモリ
	unique_ptr<GraphicsMemory> _gmemory;		
	//フォント表示用オブジェクト
	unique_ptr<SpriteFont> _spriteFont;
	//スプライト（画像）表示用オブジェクト
	unique_ptr<SpriteBatch> _spriteBatch;

	//ピクセルシェーダー用データ
	ID3DBlob* _psBlob = nullptr;											

	//ウィンドウサイズ
	SIZE _winSize;															
	//画面の幅
	LONG _width;															
	//画面の高さ
	LONG _height;															

	unique_ptr<BasicEffect> _effect;

	unique_ptr<PrimitiveBatch<VertexPositionColor>> _batch;

	//ワールド行列
	SimpleMath::Matrix _world;												
	//ビュー行列
	SimpleMath::Matrix _view;												
	//プロジェクション行列
	SimpleMath::Matrix _proj;												

	//フォント・画像用ヒープ
	ComPtr<ID3D12DescriptorHeap> _heapForSpriteFont = nullptr;				
	//ヒープハンドル(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								
	//ヒープハンドル(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								
	//ハンドルのアドレスの差分
	UINT _incrementSize;													

	//ハンドル(GPU)のベクトル
	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					

	unique_ptr<BoxCollider2D> _startColl;
	unique_ptr<BoxCollider2D> _exitColl;

	//ロード中アイコン用矩形
	RECT _loadingRect;
	//背景用矩形
	RECT _BGRect;														

	//スプライト描画用ルートシグネチャ
	ComPtr<ID3D12RootSignature> _spriteRS;									

	//X軸のグリッドの幅
	SimpleMath::Vector3 _XAxis = SimpleMath::Vector3(1000.f, 0.f, 0.0f);	
	//Y軸のグリッドの幅
	SimpleMath::Vector3 _YAxis = SimpleMath::Vector3(0.0f, 1000.f, 0.0f);	
	//Z軸のグリッドの幅
	SimpleMath::Vector3 _ZAxis = SimpleMath::Vector3(0.0f, 0.0f, 1000.f);	
	//座標の中心
	SimpleMath::Vector3 _origin = SimpleMath::Vector3::Zero;				

	//コンストラクタ
	SpriteManager();
	SpriteManager(const SpriteManager&) = delete;
	//デストラクタ
	~SpriteManager();

	//SpriteBatch用ルートシグネチャを作成する関数
	HRESULT CreateSpriteRS();												
	//UI用の画像のバッファー・ビューを作成する関数
	HRESULT CreateUIBufferView(const wchar_t* path, string key);

	//右手座標を左手座標に変換する関数
	XMVECTOR RightPosToLeftPos(const Vector3& rightPos);

	//スプライト・文字列表示用オブジェクトを初期化する関数
	void InitSpriteDevices();												

	//当たり判定を描画する関数
	void ColliderDraw(const shared_ptr<BoxCollider> collider);

	//マウスカーソルが矩形内にあるか確認する
	bool IsCursorInRect(RECT rect);
public:
	static SpriteManager& Instance();

	void Init(LONG width, LONG height);

	//画面サイズの変更を感知して矩形を調整する
	void AdjustSpriteRect();										

	//ロード画面での描画
	void LoadingDraw();		

	//タイトル画面でのボタンの描画
	void ButtonDraw();

	//背景の描画
	void BackGroundDraw();

	//マウスカーソルを描画
	void GridDraw(const map<string, shared_ptr<IFBX>> actorAndObjs);

	//グラフィックスメモリをコマンドリストにセット
	void Commit();

	//カーソルがスタートボタン上にあるか
	bool TitleIsOnStart();

	//カーソルが終了ボタン上にあるか
	bool TitleIsOnExit();
};