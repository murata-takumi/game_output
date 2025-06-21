#pragma once
#include "FBX/FBXBase.h"

/// <summary>
/// FBX�I�u�W�F�N�g
/// Actor�Ƃ͈Ⴂ�����Ȃ���
/// </summary>
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class ImGuiManager;
class Vector3;
class FBXObject : public FBXBase
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;
private:
	//���W�ϊ��o�b�t�@�[�E�r���[���쐬����֐�
	HRESULT CreateTransformView()override;

public:
	//�R���X�g���N�^
	FBXObject(const wchar_t* filePath,const string name,Vector3 size,Vector3 pos = Vector3(0.0f, 0.0f, 0.0f));
	//�f�X�g���N�^
	~FBXObject();

	//�X�V�p�֐�
	void Update()override;
};
