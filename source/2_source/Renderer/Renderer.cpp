#include "Functions.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
Renderer::Renderer(Dx12Wrapper& dx12):_dx12(dx12)
{
	CreateRootSignature();						//���[�g�V�O�l�`��������
	CreateGraphicsPipelineForPMD();				//�p�C�v���C���X�e�[�g������
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Renderer::~Renderer()
{

}

/// <summary>
/// ���[�g�V�O�l�`��������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Renderer::CreateRootSignature()
{
	//�V�F�[�_�[�̃X���b�g�ƕR�t���f�B�X�N���v�^�����W�̍쐬
	CD3DX12_DESCRIPTOR_RANGE descTblRange[3] = {};
	descTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);		//�萔[b0]
	descTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		//�萔[b1]
	descTblRange[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);		//�e�N�X�`��[t0]

	//�o�b�t�@�[�ƃf�B�X�N���v�^�����W��R�Â��郋�[�g�p�����[�^�̍쐬
	CD3DX12_ROOT_PARAMETER rootparam[3] = {};
	rootparam[0].InitAsDescriptorTable(1, &descTblRange[0]);			//�r���[�E�v���W�F�N�V�����p�o�b�t�@�[���Ǘ�
	rootparam[1].InitAsDescriptorTable(1, &descTblRange[1]);			//���W�ϊ��p�o�b�t�@�[
	rootparam[2].InitAsDescriptorTable(1, &descTblRange[2],				//�e�N�X�`���p�o�b�t�@�[
		D3D12_SHADER_VISIBILITY_PIXEL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDescs = {};						//�T���v���[�i�e�N�X�`����XY���W�̎擾�@�����߂�j�̍쐬
	samplerDescs.Init(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};					//�ݒ�p�\���̂̍쐬�E�ݒ�
	rootSignatureDesc.Init(
		3,
		rootparam,
		1,
		&samplerDescs,
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

	result = _dx12.Device()->CreateRootSignature(						//���[�g�V�O�l�`���̍쐬
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

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =											//GPU�ɒ��_�f�[�^���ǂ����߂��邩������
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"TANGENT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"IDs",0,DXGI_FORMAT_R32G32B32A32_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"WEIGHT",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline = {};									//�p�C�v���C���X�e�[�g�ݒ�p�\����
	gPipeline.pRootSignature = _rootSignature.Get();									//���[�g�V�O�l�`���Z�b�g

#ifdef _DEBUG

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

#else

	gPipeline.VS.pShaderBytecode = g_FBXVS;												//���_�V�F�[�_�[�̃o�C�i���ƃ������T�C�Y
	gPipeline.VS.BytecodeLength = sizeof(g_FBXVS);
	gPipeline.PS.pShaderBytecode = g_FBXPS;												//�s�N�Z���V�F�[�_�[
	gPipeline.PS.BytecodeLength = sizeof(g_FBXPS);

#endif

	gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;									//�T���v���}�X�N

	gPipeline.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);							//�u�����h�X�e�[�g
	
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

	result = _dx12.Device()->CreateGraphicsPipelineState(&gPipeline,
		IID_PPV_ARGS(_pipeline.ReleaseAndGetAddressOf()));								//�p�C�v���C���X�e�[�g�쐬

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