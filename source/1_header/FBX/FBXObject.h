#pragma once
#include "FBX/FBXBase.h"

/// <summary>
/// FBXオブジェクト
/// Actorとは違い動かない物
/// </summary>
class AssimpLoader;
class Dx12Wrapper;
class ImGuiManager;
class FBXObject : public FBXBase
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;
private:
	//座標変換バッファー・ビューを作成する関数
	HRESULT CreateTransformView()override;

public:
	//コンストラクタ
	FBXObject(const wchar_t* filePath,const string name,XMFLOAT3 size,XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f));
	//デストラクタ
	~FBXObject();

	//更新用関数
	void Update()override;
};
