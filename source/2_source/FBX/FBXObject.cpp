#include "Functions.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="filePath">���f���p�X</param>
/// <param name="pos">�������W</param>
FBXObject::FBXObject(const wchar_t* filePath, XMFLOAT3 size, XMFLOAT3 pos)
	:FBXBase(filePath, size, pos)
{
	//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬
	CreateTransformView();
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
		IID_PPV_ARGS(FBXBase::_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}
	result = FBXBase::_transBuffer->Map(0, nullptr, (void**)&(FBXBase::_mappedMats));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	FBXBase::_mappedMats[0] = XMMatrixIdentity();
	//���s�ړ�
	FBXBase::_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);

	_collider->Update(FBXBase::_mappedMats[0]);

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(FBXBase::_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�r���[�̍쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									
	cbvDesc.BufferLocation = FBXBase::_transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(FBXBase::_transBuffer->GetDesc().Width);

	Dx12Wrapper::Instance().Device()->CreateConstantBufferView(&cbvDesc, FBXBase::_transHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// ���t���[���̍��W�ϊ����������s����֐�
/// </summary>
void
FBXObject::Update()
{
	//TODO:�����Ɉړ�������������������
	//���������Ȃ��Ɠ����蔻��̒��S���I�u�W�F�N�g���ɂȂ��Ă��܂�
	_collider->Update(XMMatrixTranslation(0,_collider->HalfLength().y, 0) * FBXBase::_mappedMats[0]);
}