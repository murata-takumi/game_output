#pragma once
#include "Application.h"

/// <summary>
/// FBXモデルの派生元となるインターフェース
/// </summary>
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class ICollider;
class ImGuiManager;
class Vector3;
class aiScene;
class Mesh;
class BoneInfo;
class IFbx
{
protected:
	//座標変換行列を作成
	virtual HRESULT CreateTransformView() = 0;
public:
	//コンストラクタ
	IFbx() = default;
	//デストラクタ
	~IFbx() = default;

	//初期化関数
	virtual HRESULT Init(const wchar_t* filePath, const string name, 
		const Vector3& size, const Vector3& pos,ColliderType colType) = 0;

	//当たり判定を返す
	virtual shared_ptr<ICollider>  Collider() = 0;

	//描画関数
	virtual void Draw() = 0;

	//更新関数
	virtual void Update() = 0;

	//現在の座標を返す
	virtual Vector3 CurrentPosition() = 0;
};