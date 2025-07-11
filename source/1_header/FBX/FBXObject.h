#pragma once
#include "FBX/IFbx.h"

/// <summary>
/// FBXオブジェクト
/// Actorとは違い動かない物
/// </summary>
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class FbxComposition;
class ICollider;
class ImGuiManager;
class Vector3;
class FbxObject : public IFbx
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;
private:
	//座標変換バッファー・ビューを作成する関数
	HRESULT CreateTransformView()override;

	//FBXクラスの共通処理をまとめたインスタンス
	shared_ptr<FbxComposition> _fbxComp;

public:
	//コンストラクタ
	FbxObject() = default;
	//デストラクタ
	~FbxObject() = default;

	//初期化関数
	HRESULT Init(const wchar_t* filePath, const string name, 
				const Vector3& size, const Vector3& pos, ColliderType colType)override;

	//当たり判定を取得
	shared_ptr<ICollider>  Collider()override;

	//描画処理
	void Draw()override;

	//更新処理
	void Update()override;

	//現在の座標を取得
	Vector3 CurrentPosition()override;
};
