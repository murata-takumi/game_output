#include "Functions.h"
#include "FBX/FBXBase.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="filePath">���f���i�[�t�@�C����</param>
/// <param name="size">�����蔻��̑傫��</param>
/// <param name="pos">�������W</param>
/// <param name="diff">�����蔻��̍���</param>
FBXBase::FBXBase(const wchar_t* filePath, const XMFLOAT3& size, const XMFLOAT3& pos, const XMFLOAT3& diff)
	:_pos(XMLoadFloat3(&pos))
{
	_collectNormal = true;

	//���f���֘A�̏���������
	InitModel(filePath);														

	//���_�o�b�t�@�[�E�r���[�쐬
	CreateVertexBufferView();													
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬
	CreateIndexBufferView();													
	//�V�F�[�_�[���\�[�X�E�r���[�쐬
	CreateShaderResourceView();													

	//�����蔻����쐬
	_collider = make_shared<BoxCollider>(size, diff);
}

/// <summary>
/// �f�X�g���N�^
/// �|�C���^���J������
/// </summary>
FBXBase::~FBXBase()
{

}

/// <summary>
/// ���f���֘A�̏��������s���֐�
/// </summary>
/// <param name="filePath">���f���̃p�X</param>
void
FBXBase::InitModel(const wchar_t* filePath)
{
	//���f���ǂݍ��ݗp�ݒ�
	ImportSettings settings =										
	{
		//���b�V�����
		_meshes,				
		//�{�[�����ƃC���f�b�N�X�̘A�z�z��
		_boneMapping,												
		//�s��Ȃǂ̃{�[�����̃x�N�g��
		_boneInfo,													
		//U�EV���W�𔽓]�����邩
		false,														
		true,
	};

	//���f����񂪊i�[����Ă���V�[����ǂݍ���
	auto& loader = AssimpLoader::Instance();
	_scene = loader.LoadScene(filePath);
	//�V�[�������Ƀ��f����ǂݍ���
	if (_scene != nullptr)
	{
		loader.LoadModel(_scene, filePath, settings);
	}
	else
	{
		assert(0);
	}
}

/// <summary>
/// ���b�V�����ɒ��_�o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
FBXBase::CreateVertexBufferView()
{
	//�Ԃ�l��������
	result = S_OK;											

	_normals.emplace_back(_meshes[0].vertices[0]);

	//�r���[�������b�V�����ɍ��킹��
	_vbViews.reserve(_meshes.size());													
	//�S���b�V���ɑ΂����������s
	for (size_t i = 0; i < _meshes.size(); i++)											
	{
		//�i�[�p�o�b�t�@�[�E�r���[
		ID3D12Resource* tmpVertexBuffer = nullptr;										
		D3D12_VERTEX_BUFFER_VIEW tmpVBView = {};

		//���_�S�̂̃f�[�^�T�C�Y
		auto size = sizeof(FBXVertex) * _meshes[i].vertices.size();

		if (_collectNormal)
		{
			for (auto& vert : _meshes[i].vertices)
			{
				_normals.emplace_back(vert);
				auto vec = XMLoadFloat3(&vert.normal);

				for (auto& a : _normals)
				{
					auto aVec = XMLoadFloat3(&a.normal);
					auto b = fabs(1.0f - XMVector3Dot(vec, aVec).m128_f32[0]);
					if (b <= FLT_EPSILON)
					{
						_normals.pop_back();
						break;
					}
				}
			}
		}

		//�q�[�v�v���p�e�B
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);				
		//���\�[�X�ݒ�
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);								

		//�o�b�t�@�[�쐬
		result = Dx12Wrapper::Instance().Device()->CreateCommittedResource								
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpVertexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//���_�f�[�^�擾���A�o�b�t�@�[�֏�������
		vector<FBXVertex> data = _meshes[i].vertices;
		FBXVertex* mappedVertex = nullptr;
		tmpVertexBuffer->Map(0, nullptr, (void**)&mappedVertex);
		copy(begin(data), end(data), mappedVertex);
		tmpVertexBuffer->Unmap(0, nullptr);

		//�r���[�Ƀo�b�t�@�[������������
		tmpVBView.BufferLocation = tmpVertexBuffer->GetGPUVirtualAddress();				
		tmpVBView.SizeInBytes = static_cast<UINT>(tmpVertexBuffer->GetDesc().Width);
		tmpVBView.StrideInBytes = sizeof(FBXVertex);

		//�r���[���x�N�g���Ɋi�[
		_vbViews.push_back(tmpVBView);
	}

	return result;
}

/// <summary>
/// ���b�V�����ɃC���f�b�N�X�o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <returns></returns>
HRESULT
FBXBase::CreateIndexBufferView()
{
	//�Ԃ�l��������
	result = S_OK;

	//�C���f�b�N�X�o�b�t�@�[�r���[��p�ӂ���									
	_ibViews.reserve(_meshes.size());

	//�S���b�V���ɑ΂����������s
	for (size_t i = 0; i < _meshes.size(); i++)										
	{
		//�i�[�p�o�b�t�@�[�E�r���[
		ID3D12Resource* tmpIndexBuffer = nullptr;									
		D3D12_INDEX_BUFFER_VIEW tmpIBView = {};

		//�C���f�b�N�X�S�̂̃f�[�^�T�C�Y
		auto size = sizeof(uint32_t) * _meshes[i].indices.size();					

		//�q�[�v�v���p�e�B
		auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);			
		//���\�[�X�ݒ�
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);							

		//�o�b�t�@�[�쐬
		result = Dx12Wrapper::Instance().Device()->CreateCommittedResource							
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&tmpIndexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//�C���f�b�N�X�f�[�^���擾�E�o�b�t�@�[�ɏ�������
		vector<uint32_t> data = _meshes[i].indices;
		uint32_t* mappedIndex = nullptr;
		tmpIndexBuffer->Map(0, nullptr, (void**)&mappedIndex);
		copy(begin(data), end(data), mappedIndex);
		tmpIndexBuffer->Unmap(0, nullptr);

		//�r���[�Ƀo�b�t�@�[������������
		tmpIBView.BufferLocation = tmpIndexBuffer->GetGPUVirtualAddress();			
		tmpIBView.Format = DXGI_FORMAT_R32_UINT;
		tmpIBView.SizeInBytes = static_cast<UINT>(size);

		//�C���f�b�N�X�o�b�t�@�[�r���[��z��ɒǉ�								
		_ibViews.push_back(tmpIBView);
	}

	return result;
}

/// <summary>
/// �I�u�W�F�N�g�̃e�N�X�`���ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXBase::CreateShaderResourceView()
{
	//�Ԃ�l��������
	result = S_OK;

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};												
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = static_cast<UINT>(_meshes.size());
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(_texHeap.ReleaseAndGetAddressOf()));

	//SRV�p�\���̂̍쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};												
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v�̐擪�A�h���X(CPU)
	auto CPUHeapHandle = _texHeap->GetCPUDescriptorHandleForHeapStart();						
	//�q�[�v�̐擪�A�h���X(GPU)	
	auto GPUHeapHandle = _texHeap->GetGPUDescriptorHandleForHeapStart();					
	//�擪�A�h���X�̂��炷��
	auto incrementSize =	
		Dx12Wrapper::Instance().Device()->GetDescriptorHandleIncrementSize(texHeapDesc.Type);

	//�e�N�X�`���ǂݍ��ݗp�f�[�^
	TexMetadata meta = {};																		
	ScratchImage scratch = {};
	DXGI_FORMAT format;																			
	size_t width;
	size_t height;
	UINT16 arraySize;
	UINT16 mipLevels;
	void* pixels;
	UINT rowPitch;
	UINT slicePitch;

	//�e���b�V���ɑ΂��e�N�X�`���̓ǂݍ��ݏ���
	for (size_t i = 0; i < _meshes.size(); i++)													
	{
		ID3D12Resource* tmpTexBuffer = nullptr;

		//�e�N�X�`���̃p�X
		wstring path = _meshes[i].diffuseMap;													
		//�e�N�X�`��������������D�F�e�N�X�`��������
		if (wcscmp(path.c_str(), L"") == 0)														
		{
			vector<unsigned char> data(4 * 4 * 4);
			fill(data.begin(), data.end(), 0x80);

			format = DXGI_FORMAT_R8G8B8A8_UNORM;
			width = 4;
			height = 4;
			arraySize = 1;
			mipLevels = 1;

			pixels = data.data();
			rowPitch = 4 * 4;
			slicePitch = static_cast<UINT>(data.size());
		}
		//�ʏ�e�N�X�`��
		else																					
		{
			//�g���q���擾
			auto ext = FileExtension(path);														
			//�g���q��"psd"�������ꍇ�A"tga"�ɕϊ�����
			if (wcscmp(ext.c_str(), L"psd") == 0)												
			{
				path = ReplaceExtension(path, "tga");
				ext = FileExtension(path);
			}

			//�g���q�ɉ����ēǂݍ��݊֐���ς���
			result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](path, &meta, scratch);				
			if (FAILED(result))
			{
				assert(0);
				return result;
			}

			//�e�N�X�`���f�[�^�̗p��
			auto img = scratch.GetImage(0, 0, 0);												
			format = meta.format;
			width = meta.width;
			height = meta.height;
			arraySize = static_cast<UINT16>(meta.arraySize);
			mipLevels = static_cast<UINT16>(meta.mipLevels);
			pixels = img->pixels;
			rowPitch = static_cast<UINT>(img->rowPitch);
			slicePitch = static_cast<UINT>(img->slicePitch);
		}

		//�o�b�t�@�[�p�q�[�v�v���p�e�B
		auto heapProp =
			CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);	
		//���\�[�X�ݒ�
		auto desc = CD3DX12_RESOURCE_DESC::Tex2D(
			format,
			width,
			height,
			arraySize,
			mipLevels);

		//�o�b�t�@�[�̍쐬
		result = Dx12Wrapper::Instance().Device()->CreateCommittedResource										
		(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			nullptr,
			IID_PPV_ARGS(&tmpTexBuffer)
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//�e�N�X�`���̏�������
		result = tmpTexBuffer->WriteToSubresource(0,											
			nullptr,
			pixels,
			rowPitch,
			slicePitch
		);
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		//�r���[�̃t�H�[�}�b�g���e�N�X�`���ɍ��킹�A�쐬
		srvDesc.Format = tmpTexBuffer->GetDesc().Format;										
		Dx12Wrapper::Instance().Device()->CreateShaderResourceView(tmpTexBuffer, &srvDesc, CPUHeapHandle);

		//GPU�̃A�h���X��ǉ�
		_gpuHandles.push_back(GPUHeapHandle);													

		//CPU�EGPU�̃A�h���X�����炷
		CPUHeapHandle.ptr += incrementSize;														
		GPUHeapHandle.ptr += incrementSize;
	}

	return result;
}

/// <summary>
/// ���t���[���̕`�揈�������s����֐�
/// </summary>
void
FBXBase::Draw()
{
	//���W�ϊ��p�f�B�X�N���v�^�q�[�v���Z�b�g
	ID3D12DescriptorHeap* transformHeaps[] = { _transHeap.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, transformHeaps);

	//���[�g�p�����[�^�ƃf�B�X�N���v�^�q�[�v�̃n���h�����֘A�t��
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(1, _transHeap->GetGPUDescriptorHandleForHeapStart());

	//���f�����\�����郁�b�V�����Ɉȉ��̏������s��
	for (size_t i = 0; i < _meshes.size(); i++)
	{
		//���_�E�C���f�b�N�X�o�b�t�@�[�r���[�̃Z�b�g
		Dx12Wrapper::Instance().CommandList()->IASetVertexBuffers(0, 1, &_vbViews[i]);
		Dx12Wrapper::Instance().CommandList()->IASetIndexBuffer(&_ibViews[i]);

		//�e�N�X�`���o�b�t�@�[�r���[�̃Z�b�g
		ID3D12DescriptorHeap* SetTexHeap[] = { _texHeap.Get() };
		Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, SetTexHeap);
		Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(2, _gpuHandles[i]);

		//���b�V���̕`��
		Dx12Wrapper::Instance().CommandList()->DrawIndexedInstanced(static_cast<UINT>(_meshes[i].indices.size()), 1, 0, 0, 0);
	}
}

/// <summary>
/// �����蔻���Ԃ�
/// </summary>
/// <returns>�����蔻��</returns>
shared_ptr<BoxCollider>
FBXBase::Collider()const
{
	return _collider;
}