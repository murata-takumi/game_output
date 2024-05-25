#include "Functions.h"
#include "Renderer/PeraRenderer.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �y���|���S���p���\�[�X�ERTV�ESRV���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
PeraRenderer::CreatePeraResourcesAndView()
{
	auto resDesc = _dx12.BackBuffer()->GetDesc();							//���X�g�p���Ă������\�[�X�ݒ�p�\���̂𗘗p

	float clsClr[4] = { 0.25f,0.25f,0.25f,1.0f };							//�N���A�l�̓����_�����O���Ɠ����l��ݒ肷��K�v������
	D3D12_CLEAR_VALUE clearValue = 
		CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM, clsClr);

	auto result = _dx12.Device()->CreateCommittedResource(					//���\�[�X(�����_�[�^�[�Q�b�g)�̍쐬
		&_defHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&_peraResource));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	auto heapDesc = _dx12.RTVHeap()->GetDesc();								//RTV�p�q�[�v�p�\���̂̐ݒ�
	heapDesc.NumDescriptors = 1;

	result = _dx12.Device()->CreateDescriptorHeap(							//RTV�p�q�[�v�̍쐬
		&heapDesc, IID_PPV_ARGS(&_peraRTVHeap));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_RENDER_TARGET_VIEW_DESC peraRTVDesc = {};							//RTV�p�\���̂̐ݒ�
	peraRTVDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	peraRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	_dx12.Device()->CreateRenderTargetView(									//RTV�̍쐬
		_peraResource.Get(),
		&peraRTVDesc,
		_peraRTVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	heapDesc.NumDescriptors = 1;											//SRV�p�q�[�v�p�\���̂̐ݒ�
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	
	result = _dx12.Device()->CreateDescriptorHeap(							//SRV�p�q�[�v�̍쐬
		&heapDesc, IID_PPV_ARGS(_peraSRVHeap.ReleaseAndGetAddressOf()));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC peraSRVDesc = {};						//SRV�p�\���̂̍쐬
	peraSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	peraSRVDesc.Format = peraRTVDesc.Format;
	peraSRVDesc.Texture2D.MipLevels = 1;
	peraSRVDesc.Shader4ComponentMapping
		= D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	_dx12.Device()->CreateShaderResourceView(								//SRV�̍쐬
		_peraResource.Get(),
		&peraSRVDesc,
		_peraSRVHeap->GetCPUDescriptorHandleForHeapStart()
	);

	return result;
}

/// <summary>
/// �y���|���S���p���_�o�b�t�@�[�EVBV���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
PeraRenderer::CreatePeraVertex()
{
	struct PeraVertex											//�y���|���S�����쐬���邽�߂̒��_�\����
	{
		XMFLOAT3 pos;
		XMFLOAT2 uv;
	};

	PeraVertex pv[] =											//�y���|���S�����\������4�̒��_
	{
		{{-1.0f,-1.0f,0.1f},{0.0f,1.0f}},
		{{-1.0f,1.0f,0.1f},{0.0f,0.0f}},
		{{1.0f,-1.0f,0.1f},{1.0f,1.0f}},
		{{1.0f,1.0f,0.1f},{1.0f,0.0f}}
	};

	auto pvResDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(pv));	//���\�[�X�̃T�C�Y��ݒ�

	auto result = _dx12.Device()->CreateCommittedResource(		//�y���|���S���p���_�o�b�t�@�[�̍쐬
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&pvResDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_peraVB));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	PeraVertex* mappedPera = nullptr;							//�y���|���S����GPU���ɓ]���ł���悤�ɂ���
	_peraVB->Map(0, nullptr, (void**)&mappedPera);
	copy(begin(pv), end(pv), mappedPera);
	_peraVB->Unmap(0, nullptr);

	_peraVBV.BufferLocation = _peraVB->GetGPUVirtualAddress();	//���_�o�b�t�@�[�r���[�̐ݒ�
	_peraVBV.SizeInBytes = sizeof(pv);
	_peraVBV.StrideInBytes = sizeof(PeraVertex);

	return result;
}

/// <summary>
/// �y���|���S���p�p�C�v���C�����쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
PeraRenderer::CreatePeraPipeline()
{
	D3D12_INPUT_ELEMENT_DESC layout[2] =												//�y���|���S���p���_���C�A�E�g
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,									//���_���W
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,									//UV���W
			D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc = {};									//�y���|���S���p�p�C�v���C���p�\���̂̍쐬
	gpsDesc.InputLayout.NumElements = _countof(layout);									//���̓��C�A�E�g��
	gpsDesc.InputLayout.pInputElementDescs = layout;									//���̓��C�A�E�g�ݒ�

	ID3DBlob* errorBlob = nullptr;														//�G���[�m�F�p�f�[�^

	auto result = S_OK;																	//�֐��̕Ԃ�l�����炩���ߐݒ�

#ifdef _DEBUG																			//Debug�r���h���̓t�@�C�����璼�ړǂݍ���

	ID3DBlob* vs = nullptr;																//���_�V�F�[�_�[�p�f�[�^
	ID3DBlob* ps = nullptr;																//�s�N�Z���V�F�[�_�[�p�f�[�^

	result = D3DCompileFromFile(L"3_shader/Pera/PeraVertex.hlsl", nullptr,				//���_�V�F�[�_�[�ǂݍ���
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraVS", "vs_5_0", 0, 0,
		&vs,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	result = D3DCompileFromFile(L"3_shader/Pera/PeraPixel.hlsl", nullptr,				//�s�N�Z���V�F�[�_�[�ǂݍ���
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"PeraPS", "ps_5_0", 0, 0,
		&ps,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	gpsDesc.VS = CD3DX12_SHADER_BYTECODE(vs);											//�p�C�v���C���p�\���̂ɒ��_�V�F�[�_�[��ݒ�
	gpsDesc.PS = CD3DX12_SHADER_BYTECODE(ps);											//�s�N�Z���V�F�[�_�[���ݒ�

#else																					//Release�r���h���̓R���p�C���ς݃V�F�[�_�[��ǂݍ���

	gpsDesc.VS.pShaderBytecode = g_PeraVS;												//���_�V�F�[�_�[�̃o�C�i���ƃ������T�C�Y
	gpsDesc.VS.BytecodeLength = sizeof(g_PeraVS);
	gpsDesc.PS.pShaderBytecode = g_PeraPS;												//�s�N�Z���V�F�[�_�[
	gpsDesc.PS.BytecodeLength = sizeof(g_PeraPS);

#endif

	gpsDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);								//�p�C�v���C���X�e�[�g�̐ݒ�
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	D3D12_DESCRIPTOR_RANGE range[2] = {};												//�f�B�X�N���v�^�����W
	range[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;								//��ʃe�N�X�`��[t0]
	range[0].BaseShaderRegister = 0;
	range[0].NumDescriptors = 1;

	range[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;								//�萔�X���b�g[b0]
	range[1].BaseShaderRegister = 0;
	range[1].NumDescriptors = 1;

	D3D12_ROOT_PARAMETER rp[2] = {};													//���[�g�p�����[�^
	rp[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rp[0].DescriptorTable.pDescriptorRanges = &range[0];
	rp[0].DescriptorTable.NumDescriptorRanges = 1;

	rp[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rp[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rp[1].DescriptorTable.pDescriptorRanges = &range[1];
	rp[1].DescriptorTable.NumDescriptorRanges = 1;

	D3D12_STATIC_SAMPLER_DESC sampler = CD3DX12_STATIC_SAMPLER_DESC(0);					//�T���v���[

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};												//���[�g�V�O�l�`���ݒ�\���̂̍쐬
	rsDesc.NumParameters = 2;
	rsDesc.pParameters = rp;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.pStaticSamplers = &sampler;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* rsBlob = nullptr;															//���[�g�V�O�l�`���p�f�[�^

	result = D3D12SerializeRootSignature(												//���[�g�V�O�l�`���p�f�[�^�̏�����
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&rsBlob,
		&errorBlob);
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	result = _dx12.Device()->CreateRootSignature(										//���[�g�V�O�l�`���̍쐬
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(&_peraRS));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	rsBlob->Release();																	//�s�v�ɂȂ����f�[�^���J��

	gpsDesc.pRootSignature = _peraRS.Get();												//�p�C�v���C���փ��[�g�V�O�l�`����o�^

	result = _dx12.Device()->CreateGraphicsPipelineState(								//�p�C�v���C���̍쐬
		&gpsDesc, IID_PPV_ARGS(&_peraPipeline));
	if (result != S_OK)
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
PeraRenderer::PeraRenderer(Dx12Wrapper& dx12):_dx12(dx12)
{
	CreatePeraResourcesAndView();							//�y���|���S���pRT�ERTV�ESRV���쐬
	 
	CreatePeraVertex();										//�y���|���S���p���_�o�b�t�@�[�EVBV���쐬
	
	CreatePeraPipeline();									//�y���|���S���p�p�C�v���C�����쐬
}

/// <summary>
/// �f�X�g���N�^
/// ���ɏ����͂��Ȃ�
/// </summary>
PeraRenderer::~PeraRenderer()
{

}

/// <summary>
/// �y���|���S���p���\�[�X�̑J��(SHADER_RESOURCE��RENDER_TARGET)�ERTV�̃Z�b�g�����s����֐�
/// </summary>
void
PeraRenderer::BeginPeraDraw()
{
	_dx12.BarrierTransition(																//�y���|���S���p���\�[�X��RENDER_TARGET�ɑJ��
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _peraRTVHeap->GetCPUDescriptorHandleForHeapStart();							//�y���|���S���pRTV�ESRV�q�[�v�̃n���h�����擾
	auto dsvH = _dx12.DSVHeap()->GetCPUDescriptorHandleForHeapStart();						//DSV�q�[�v�̃n���h�����擾

	_dx12.CommandList()->OMSetRenderTargets(1, &rtvH, false, &dsvH);						//RTV�EDSV�q�[�v�̃n���h�����R�}���h���X�g�ɐݒ�

	float clearColor[4] = { 0.25f,0.25f,0.25f,1.0f };										//�w�i�F��ݒ�

	_dx12.CommandList()->ClearRenderTargetView(												//RTV���N���A
		rtvH, clearColor, 0, nullptr);				
	_dx12.CommandList()->ClearDepthStencilView(												//DSV���N���A
		dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	_dx12.CommandList()->RSSetViewports(1, _dx12.ViewPort());								//�r���[�|�[�g���R�}���h���X�g�ɐݒ�
	_dx12.CommandList()->RSSetScissorRects(1, _dx12.Rect());								//�V�U�[��`���R�}���h���X�g�ɐݒ�
}

/// <summary>
/// �y���|���S���p���[�g�V�O�l�`���E�p�C�v���C�����Z�b�g����֐�
/// </summary>
void
PeraRenderer::SetPeraPipelines()
{
	_dx12.SetPipelines(																//�p�C�v���C���X�e�[�g�A���[�g�V�O�l�`���A�v���~�e�B�u�g�|���W�[���Z�b�g
		_peraRS.Get(), _peraPipeline.Get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D12DescriptorHeap* peraSRVHeaps[] = { _peraSRVHeap.Get() };					//�y���|���S���pSRV�A�y�уn���h����ݒ�
	_dx12.CommandList()->SetDescriptorHeaps(1, peraSRVHeaps);
	auto handle = _peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(0, handle);

	ID3D12DescriptorHeap* factorCBVHeap[] = { _dx12.FactorCBVHeap() };				//�G�t�F�N�g�K�p�f�[�^�pCBV�A�y�уn���h����ݒ�
	_dx12.CommandList()->SetDescriptorHeaps(1, factorCBVHeap);
	handle = _dx12.FactorCBVHeap()->GetGPUDescriptorHandleForHeapStart();
	_dx12.CommandList()->SetGraphicsRootDescriptorTable(1, handle);

	_dx12.UpdateFade();																//�f�[�^���X�V
}

/// <summary>
/// �y���|���S���p���\�[�X�̑J��(RENDER_TARGET��SHADER_RESOURCE)�����s����֐�
/// </summary>
void
PeraRenderer::EndPeraDraw()
{
	_dx12.BarrierTransition(							//�y���|���S���p���\�[�X��SHADER_RESOURCE�ɑJ��
		_peraResource.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);
}

/// <summary>
/// �y���|���S���p���_�o�b�t�@�[�r���[��Ԃ��֐�
/// </summary>
/// <returns>�r���[</returns>
D3D12_VERTEX_BUFFER_VIEW*
PeraRenderer::PeraVBView()
{
	return &_peraVBV;
}

/// <summary>
/// �y���|���S���p�p�C�v���C����Ԃ��֐�
/// </summary>
/// <returns>�p�C�v���C���̃|�C���^</returns>
ID3D12PipelineState*
PeraRenderer::PeraPipeline()
{
	return _peraPipeline.Get();
}

/// <summary>
/// �y���|���S���p���[�g�V�O�l�`����Ԃ��֐�
/// </summary>
/// <returns>���[�g�V�O�l�`���̃|�C���^</returns>
ID3D12RootSignature*
PeraRenderer::PeraRootSignature()
{
	return _peraRS.Get();
}