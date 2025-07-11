#pragma once
#include "FBX/IFbx.h"

/// <summary>
/// FBX�I�u�W�F�N�g
/// Actor�Ƃ͈Ⴂ�����Ȃ���
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
	//���W�ϊ��o�b�t�@�[�E�r���[���쐬����֐�
	HRESULT CreateTransformView()override;

	//FBX�N���X�̋��ʏ������܂Ƃ߂��C���X�^���X
	shared_ptr<FbxComposition> _fbxComp;

public:
	//�R���X�g���N�^
	FbxObject() = default;
	//�f�X�g���N�^
	~FbxObject() = default;

	//�������֐�
	HRESULT Init(const wchar_t* filePath, const string name, 
				const Vector3& size, const Vector3& pos, ColliderType colType)override;

	//�����蔻����擾
	shared_ptr<ICollider>  Collider()override;

	//�`�揈��
	void Draw()override;

	//�X�V����
	void Update()override;

	//���݂̍��W���擾
	Vector3 CurrentPosition()override;
};
