#include "Functions.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �R���X�g���N�^
/// ���W�ϊ��p�o�b�t�@�[�E�r���[���쐬
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="filePath">���f���t�@�C���̃p�X</param>
FBXObject::FBXObject(Dx12Wrapper& dx12, const wchar_t* filePath, XMFLOAT3 pos)
	:FBXBase(dx12, filePath, 
		XMFLOAT3(2000.0f, 1.0f, 2000.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)
		,pos)
{
	CreateTransformView();											//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬
}

/// <summary>
/// �f�X�g���N�^
/// ���ɉ������Ȃ�
/// </summary>
FBXObject::~FBXObject()
{

}

/// <summary>
/// �I�u�W�F�N�g�̍��W�ϊ��ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXObject::CreateTransformView()
{
	result = S_OK;

	auto buffSize = sizeof(XMMATRIX);												//���[���h�s��p�o�b�t�@�[�̍쐬
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = _dx12.Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	result = _transBuffer->Map(0, nullptr, (void**)&_mappedMats);					//���W�ϊ��p�s��̏�������
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMats[0] = XMMatrixIdentity();
	_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);			//���s�ړ�

	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							//�f�B�X�N���v�^�q�[�v�ݒ�p�\���̂̍쐬
	transformDescHeapDesc.NumDescriptors = 1;										//�Ƃ肠�������[���h�ЂƂ�
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;		//�V�F�[�_�[���猩����悤�ɂ���
	transformDescHeapDesc.NodeMask = 0;												//�m�[�h�}�X�N��0��
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;			//�f�X�N���v�^�q�[�v���

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,			//�q�[�v�̍쐬
		IID_PPV_ARGS(_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									//�r���[�ݒ�p�\���̂̍쐬
	cbvDesc.BufferLocation = _transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_transBuffer->GetDesc().Width);

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,								//�r���[�̍쐬
		_transHeap->GetCPUDescriptorHandleForHeapStart());

	_collider->Update(_mappedMats[0]);											//�����蔻���������

	return result;
}

/// <summary>
/// ���t���[���̍��W�ϊ����������s����֐�
/// </summary>
void
FBXObject::Update()
{

}