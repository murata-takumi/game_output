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
/// �������֐�
/// </summary>
/// <param name="filePath">fbx�t�@�C���̃p�X</param>
/// <param name="name">�I�u�W�F�N�g��</param>
/// <param name="size">�����蔻��̃T�C�Y</param>
/// <param name="pos">�����蔻��̍��W</param>
/// <param name="colType">�����蔻��̃^�C�v</param>
/// <returns>�����������������ǂ���</returns>
HRESULT
FbxObject::Init(const wchar_t* filePath, const string name,
	const Vector3& size, const Vector3& pos, ColliderType colType)
{
	//���ʏ�����������
	_fbxComp = make_shared<FbxComposition>();

	//���f���֘A�̏���������
	_fbxComp->InitModel(filePath);

	//���_�o�b�t�@�[�E�r���[�쐬
	_fbxComp->CreateVertexBufferView();
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬
	_fbxComp->CreateIndexBufferView();
	//�V�F�[�_�[���\�[�X�E�r���[�쐬
	_fbxComp->CreateShaderResourceView();

	//�����蔻����쐬
	if (colType == ColliderType::Box)
	{
		_fbxComp->CreateBoxCollider(size, Vector3(0, 0, 0), this);
	}
	else if (colType == ColliderType::Sphere)
	{
		_fbxComp->CreateSphereCollider(25.0f, Vector3(0, 0, 0), this);
	}

	//���W��������
	_fbxComp->TransrateVector() = pos;

	//���[���h�s��͈ړ��p�̂�
	_fbxComp->CreateTransformView(1);

	return S_OK;
}

/// <summary>
/// �����蔻����擾
/// </summary>
/// <returns>�X�}�[�g�|�C���^</returns>
shared_ptr<ICollider> 
FbxObject::Collider()
{
	return _fbxComp->Collider();
}

/// <summary>
/// �`�揈��
/// </summary>
void 
FbxObject::Draw()
{
	_fbxComp->Draw();
}

/// <summary>
/// �X�V����
/// </summary>
void 
FbxObject::Update()
{
	//�ړ�����
	_fbxComp->Update();
}

/// <summary>
/// ���݂̍��W���擾����֐�
/// </summary>
/// <returns>���W</returns>
Vector3
FbxObject::CurrentPosition()
{
	return _fbxComp->CurrentPosition();
}