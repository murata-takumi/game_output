#include "Functions.h"
#include "FBX/FBXObject.h"
#include "Wrapper/Dx12Wrapper.h"

//�����蔻��̃T�C�Y
const XMFLOAT3 COL_SIZE = XMFLOAT3(2000.0f, 1.0f, 2000.0f);

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="filePath">���f���p�X</param>
/// <param name="pos">�������W</param>
FBXObject::FBXObject(Dx12Wrapper& dx12, const wchar_t* filePath, XMFLOAT3 pos)
	:FBXBase(dx12, filePath,COL_SIZE, pos)
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
	result = _transBuffer->Map(0, nullptr, (void**)&_mappedMats);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMats[0] = XMMatrixIdentity();
	//���s�ړ�
	_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);		

	_collider->Update(_mappedMats[0]);

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�r���[�̍쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};									
	cbvDesc.BufferLocation = _transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_transBuffer->GetDesc().Width);

	_dx12.Device()->CreateConstantBufferView(&cbvDesc,_transHeap->GetCPUDescriptorHandleForHeapStart());							

	return result;
}

/// <summary>
/// ���t���[���̍��W�ϊ����������s����֐�
/// </summary>
void
FBXObject::Update()
{
	//TODO:�����Ɉړ�������������������
}