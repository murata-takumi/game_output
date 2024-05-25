#pragma once
#include "FBX/FBXBase.h"

class AssimpLoader;
class Dx12Wrapper;
class ImGuiManager;
class FBXObject : public FBXBase
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;
private:
	HRESULT CreateTransformView()override;

public:
	FBXObject(Dx12Wrapper& dx12, const wchar_t* filePath,XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f));
	~FBXObject();

	void Update()override;
};
