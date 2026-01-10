#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/ICollider.h"
#include "FBX/AssimpLoader.h"
#include "FBX/FbxComposition.h"
#include "FBX/FbxObject.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="filePath">fbxファイルのパス</param>
/// <param name="name">オブジェクト名</param>
/// <param name="size">当たり判定のサイズ</param>
/// <param name="pos">当たり判定の座標</param>
/// <param name="colType">当たり判定のタイプ</param>
/// <returns>処理が成功したかどうか</returns>
HRESULT
FbxObject::Init(const wchar_t* filePath, const string name,
	const Vector3& size, const Vector3& pos, ColliderType colType)
{
	//共通処理を初期化
	_fbxComp = make_shared<FbxComposition>();

	//モデル関連の情報を初期化
	_fbxComp->InitModel(filePath);

	//頂点バッファー・ビュー作成
	_fbxComp->CreateVertexBufferView();
	//インデックスバッファー・ビュー作成
	_fbxComp->CreateIndexBufferView();
	//シェーダーリソース・ビュー作成
	_fbxComp->CreateShaderResourceView();

	//当たり判定を作成
	switch (colType)
	{
	case ColliderType::Box:
		_fbxComp->CreateBoxCollider(size, Vector3(0, 0, 0), this);
		break;

	case ColliderType::Sphere:
		_fbxComp->CreateSphereCollider(25.0f, Vector3(0, 0, 0), this);
		break;
	case ColliderType::Capsule:
		break;
	}

	//座標も初期化
	_fbxComp->TransrateVector() = pos;

	//ワールド行列は移動用のみ
	_fbxComp->CreateTransformView(1);

	return S_OK;
}

/// <summary>
/// 当たり判定を取得
/// </summary>
/// <returns>スマートポインタ</returns>
shared_ptr<ICollider> 
FbxObject::Collider()
{
	return _fbxComp->Collider();
}

/// <summary>
/// 描画処理
/// </summary>
void 
FbxObject::Draw()
{
	_fbxComp->Draw();
}

/// <summary>
/// 更新処理
/// </summary>
void 
FbxObject::Update()
{
	//移動処理
	_fbxComp->Update();
}

/// <summary>
/// 現在の座標を取得する関数
/// </summary>
/// <returns>座標</returns>
Vector3
FbxObject::CurrentPosition()
{
	return _fbxComp->CurrentPosition();
}