#include "Functions.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

//�g�D�[���e�N�X�`���i�[�p�X
const wchar_t* TOON_PATH = L"Asset/model/textures/toon01.bmp";

/// <summary>
/// �R���X�g���N�^
/// </summary>
Renderer::Renderer()
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Renderer::~Renderer()
{

}

/// <summary>
/// �V���O���g����Ԃ�
/// </summary>
/// <returns>�V���O���g��</returns>
Renderer&
Renderer::Instance()
{
	static Renderer instance;
	return instance;
}

/// <summary>
/// �������֐�
/// </summary>
void
Renderer::Init()
{
	//���[�g�V�O�l�`��������
	CreateRootSignature();						
	//�p�C�v���C���X�e�[�g������
	CreateGraphicsPipelineForPMD();
	//�g�D�[���e�N�X�`���pSRV�쐬
	CreateToonShaderResourceView();
}

/// <summary>
/// ���[�g�V�O�l�`��������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Renderer::CreateRootSignature()
{
	//�V�F�[�_�[�̃X���b�g�ƕR�t���f�B�X�N���v�^�����W�̍쐬
	CD3DX12_DESCRIPTOR_RANGE descTblRange[4] = {};
	//�萔[b0]
	descTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);		
	//�萔[b1]
	descTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		
	//�e�N�X�`��[t0]
	descTblRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		
	//�e�N�X�`��[t1]
	descTblRange[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);		

	//�o�b�t�@�[�ƃf�B�X�N���v�^�����W��R�Â��郋�[�g�p�����[�^�̍쐬
	CD3DX12_ROOT_PARAMETER rootparam[4] = {};
	//�r���[�E�v���W�F�N�V�����p�o�b�t�@�[���Ǘ�
	rootparam[0].InitAsDescriptorTable(1, &descTblRange[0]);			
	//���W�ϊ��p�o�b�t�@�[
	rootparam[1].InitAsDescriptorTable(1, &descTblRange[1]);			
	//�e�N�X�`���p�o�b�t�@�[
	rootparam[2].InitAsDescriptorTable(1, &descTblRange[2],				
		D3D12_SHADER_VISIBILITY_PIXEL);
	//�g�D�[���e�N�X�`���p�o�b�t�@�[
	rootparam[3].InitAsDescriptorTable(1, &descTblRange[3],				
		D3D12_SHADER_VISIBILITY_PIXEL);

	//�T���v���[�i�e�N�X�`����XY���W�̎擾�@�����߂�j�̍쐬
	CD3DX12_STATIC_SAMPLER_DESC samplerDescs[2] = {};						
	//�T���v���[[s0]
	samplerDescs[0].Init(0);
	//�T���v���[[s1]
	samplerDescs[1].Init(1);
	samplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};					//�ݒ�p�\���̂̍쐬�E�ݒ�
	rootSignatureDesc.Init(
		4,
		rootparam,
		2,
		samplerDescs,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ID3DBlob* rootSigBlob = nullptr;									//���[�g�V�O�l�`���̏�����
	ID3DBlob* errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob,
		&errorBlob);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	result = Dx12Wrapper::Instance().Device()->CreateRootSignature(						//���[�g�V�O�l�`���̍쐬
		0,
		rootSigBlob->GetBufferPointer(),
		rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(_rootSignature.ReleaseAndGetAddressOf()));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	rootSigBlob->Release();												//�s�v�ɂȂ����f�[�^���J��

	return result;
}

/// <summary>
/// �O���t�B�b�N�X�p�C�v���C�����쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Renderer::CreateGraphicsPipelineForPMD()
{
	ID3DBlob* errorBlob = nullptr;														//�G���[���ʗp�I�u�W�F�N�g

	auto result = S_OK;

	//GPU�ɒ��_�f�[�^���ǂ����߂��邩������
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		//���W
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//�@��
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//�e�N�X�`��UI�l
		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//����
		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//�}�e���A��
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//�{�[��ID
		{"IDs",0,DXGI_FORMAT_R32G32B32A32_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		//�{�[���d��
		{"WEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline = {};									//�p�C�v���C���X�e�[�g�ݒ�p�\����
	gPipeline.pRootSignature = _rootSignature.Get();									//���[�g�V�O�l�`���Z�b�g

//#ifdef _DEBUG

	ID3DBlob* vsBlob = nullptr;															//���_�V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;															//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g

	result = D3DCompileFromFile(														//���_�V�F�[�_�[�ǂݍ���
		L"3_shader/FBX/FBXVertexShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"FBXVS", "vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vsBlob, &errorBlob);
	if (!CheckCompilerResult(result, errorBlob))
	{
		assert(0);
		return result;
	}

	result = D3DCompileFromFile(														//�s�N�Z���V�F�[�_�[�ǂݍ���
		L"3_shader/FBX/FBXPixelShader.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"FBXPS", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&psBlob, &errorBlob);
	if (!CheckCompilerResult(result, errorBlob))
	{
		assert(0);
		return result;
	}

	gPipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob);										//���_�V�F�[�_�[�Z�b�g
	gPipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob);										//�s�N�Z���V�F�[�_�[�Z�b�g

//#else
//
//	gPipeline.VS.pShaderBytecode = g_FBXVS;												//���_�V�F�[�_�[�̃o�C�i���ƃ������T�C�Y
//	gPipeline.VS.BytecodeLength = sizeof(g_FBXVS);
//	gPipeline.PS.pShaderBytecode = g_FBXPS;												//�s�N�Z���V�F�[�_�[
//	gPipeline.PS.BytecodeLength = sizeof(g_FBXPS);
//
//#endif

	gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;									//�T���v���}�X�N

	//�u�����h�X�e�[�g
	gPipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	gPipeline.BlendState.RenderTarget[0].BlendEnable = true;
	gPipeline.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gPipeline.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	
	gPipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);					//�|���S�����s�N�Z���ɕϊ�����d�g�݂�ݒ�
	gPipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;							//�`��s�v�ȃ|���S����`�悵�Ȃ��悤�ݒ�

	//�[�x�l����̐ݒ�
	gPipeline.DepthStencilState.DepthEnable = true;										//�[�x�l��L����
	gPipeline.DepthStencilState.StencilEnable = false;									//�X�e���V���l�͖����̂܂�
	gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;			//�[�x�o�b�t�@�[�ɐ[�x�l���������ނ悤�ݒ�
	gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;					//�s�N�Z�����m�̐[�x���r���A����������`��

	gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;										//�[�x�l��32bit�l�ŕ\��

	gPipeline.InputLayout.pInputElementDescs = inputLayout;								//���̓��C�A�E�g�ݒ�
	gPipeline.InputLayout.NumElements = _countof(inputLayout);							//���C�A�E�g�z��̗v�f��

	gPipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;			//�|���S���̐؂����
	gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;			//�|���S�����O�p�`�ŕ\������

	gPipeline.NumRenderTargets = 1;														//�����_�[�^�[�Q�b�g��
	gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;								//�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g

	gPipeline.SampleDesc.Count = 1;														//�s�N�Z��������T���v�����O��
	gPipeline.SampleDesc.Quality = 0;													//�N�I���e�B

	result = Dx12Wrapper::Instance().Device()->CreateGraphicsPipelineState(&gPipeline,
		IID_PPV_ARGS(_pipeline.ReleaseAndGetAddressOf()));								//�p�C�v���C���X�e�[�g�쐬

	return result;
}

/// <summary>
/// �g�D�[���e�N�X�`���pSRV���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Renderer::CreateToonShaderResourceView()
{
	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC texHeapDesc = {};
	texHeapDesc.NodeMask = 1;
	texHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	texHeapDesc.NumDescriptors = 1;
	texHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	auto result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&texHeapDesc,
		IID_PPV_ARGS(_toonHeap.ReleaseAndGetAddressOf()));

	//SRV�p�\���̂̍쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//�q�[�v�̐擪�A�h���X(CPU)
	auto CPUHeapHandle = _toonHeap->GetCPUDescriptorHandleForHeapStart();
	//�q�[�v�̐擪�A�h���X(GPU)	
	_GpuToonHeapHandle = _toonHeap->GetGPUDescriptorHandleForHeapStart();

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

	//�g�D�[���e�N�X�`���p�o�b�t�@�[
	ID3D12Resource* toonBuffer = nullptr;

	//�g���q���擾
	auto ext = FileExtension(TOON_PATH);

	//�g���q�ɉ����ēǂݍ��݊֐���ς���
	result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](TOON_PATH, &meta, scratch);
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
		IID_PPV_ARGS(&toonBuffer)
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//�e�N�X�`���̏�������
	result = toonBuffer->WriteToSubresource(0,
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
	srvDesc.Format = toonBuffer->GetDesc().Format;
	Dx12Wrapper::Instance().Device()->CreateShaderResourceView(toonBuffer, &srvDesc, CPUHeapHandle);

	return result;
}

/// <summary>
/// �V�F�[�_�[�ǂݍ��݂������������ǂ����𔻕ʂ���֐�
/// </summary>
/// <param name="result">�V�F�[�_�[�ǂݍ��݊֐��̕Ԃ�l</param>
/// <param name="error">�G���[�f�[�^�i�[�p�I�u�W�F�N�g</param>
/// <returns>�����������ǂ���</returns>
bool
Renderer::CheckCompilerResult(HRESULT result, ID3DBlob* error)
{
	if (FAILED(result))												//�֐������s������
	{
		
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))		//�t�@�C����������Ȃ��ꍇ���̎|���L�q
		{
			::OutputDebugStringA("�t�@�C����������܂���");
		}
		else
		{
			string errStr;											//�G���[���L�q�p������
			errStr.resize(error->GetBufferSize());					//������̃T�C�Y��ύX

			copy_n(													//�I�u�W�F�N�g�̃A�h���X����G���[�������擾���A������ɃR�s�[
				(char*)error->GetBufferPointer(),
				error->GetBufferSize(),
				errStr.begin());

			errStr += "\n";											//�����ɉ��s������������
			OutputDebugStringA(errStr.c_str());						//�o��
		}
		return false;												//�֐������s������U��Ԃ�
	}
	else
	{
		return true;												//�֐�������������^��Ԃ�
	}
}

/// <summary>
/// �`��p�֐�
/// </summary>
void
Renderer::Draw()
{
	//SRV�̃Z�b�g
	ID3D12DescriptorHeap* SetToonHeap[] = { _toonHeap.Get() };
	//�g�D�[���e�N�X�`���̃f�[�^��GPU�ɓn��
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, SetToonHeap);
	Dx12Wrapper::Instance().CommandList()->SetGraphicsRootDescriptorTable(3, _GpuToonHeapHandle);
}

/// <summary>
///	�p�C�v���C���X�e�[�g��Ԃ��֐�
/// </summary>
/// <returns>�p�C�v���C���X�e�[�g</returns>
ID3D12PipelineState*
Renderer::GetPipelineState()const
{
	return _pipeline.Get();
}

/// <summary>
/// ���[�g�V�O�l�`����Ԃ��֐�
/// </summary>
/// <returns>���[�g�V�O�l�`��</returns>
ID3D12RootSignature*
Renderer::GetRootSignature()const
{
	return _rootSignature.Get();
}