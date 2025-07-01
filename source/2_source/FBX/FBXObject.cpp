#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "FBX/AssimpLoader.h"
#include "FBX/FBXComposition.h"
#include "FBX/FBXObject.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �������֐�
/// </summary>
/// <param name="filePath">fbx�t�@�C���̃p�X</param>
/// <param name="name">�I�u�W�F�N�g��</param>
/// <param name="size">�����蔻��̃T�C�Y</param>
/// <param name="pos">�����蔻��̍��W</param>
/// <returns>�����������������ǂ���</returns>
HRESULT
FBXObject::Init(const wchar_t* filePath, const string name,
	const Vector3& size, const Vector3& pos)
{
	//���ʏ�����������
	_fbxComp = make_shared<FBXComposition>();

	//���f���֘A�̏���������
	_fbxComp->InitModel(filePath);

	//���_�o�b�t�@�[�E�r���[�쐬
	_fbxComp->CreateVertexBufferView();
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬
	_fbxComp->CreateIndexBufferView();
	//�V�F�[�_�[���\�[�X�E�r���[�쐬
	_fbxComp->CreateShaderResourceView();


	//�����蔻����쐬
	_fbxComp->CreateCollider(size, Vector3(0, 0, 0), this);

	//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬
	CreateTransformView();

	return S_OK;
}

/// <summary>
/// �I�u�W�F�N�g�̍��W�ϊ��ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXObject::CreateTransformView()
{
	HRESULT result = S_OK;

	//���[���h�s��p�o�b�t�@�[�̍쐬�E��������
	auto buffSize = sizeof(XMMATRIX);												
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_fbxComp->_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	result = _fbxComp->_transBuffer->Map(0, nullptr, (void**)&(_fbxComp->_mappedMats));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_fbxComp->_mappedMats[0] = XMMatrixIdentity();
	//���s�ړ�
	_fbxComp->_mappedMats[0] *= XMMatrixTranslationFromVector(_fbxComp->_translateVector);

	_fbxComp->Collider()->Update(_fbxComp->_mappedMats[0]);

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(_fbxComp->_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�r���[�̍쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									
	cbvDesc.BufferLocation = _fbxComp->_transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_fbxComp->_transBuffer->GetDesc().Width);

	Dx12Wrapper::Instance().Device()->CreateConstantBufferView(&cbvDesc, _fbxComp->_transHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �����蔻����擾
/// </summary>
/// <returns>�X�}�[�g�|�C���^</returns>
shared_ptr<BoxCollider> 
FBXObject::Collider()
{
	return _fbxComp->Collider();
}

/// <summary>
/// �`�揈��
/// </summary>
void 
FBXObject::Draw()
{
	_fbxComp->Draw();
}

/// <summary>
/// �X�V����
/// </summary>
void 
FBXObject::Update()
{
	_fbxComp->Update();
}

/// <summary>
/// ���݂̍��W���擾����֐�
/// </summary>
/// <returns>���W</returns>
Vector3
FBXObject::CurrentPosition()
{
	return _fbxComp->CurrentPosition();
}