#pragma once
#include "Application.h"

/// <summary>
/// FBX���f���̔h�����ƂȂ�C���^�[�t�F�[�X
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
public:
	//�R���X�g���N�^
	IFbx() = default;
	//�f�X�g���N�^
	~IFbx() = default;

	//�������֐�
	virtual HRESULT Init(const wchar_t* filePath, const string name, 
		const Vector3& size, const Vector3& pos,ColliderType colType) = 0;

	//�����蔻���Ԃ�
	virtual shared_ptr<ICollider>  Collider() = 0;

	//�`��֐�
	virtual void Draw() = 0;

	//�X�V�֐�
	virtual void Update() = 0;

	//���݂̍��W��Ԃ�
	virtual Vector3 CurrentPosition() = 0;
};