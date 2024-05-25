#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Manager/SpriteManager.h"
#include "Package/Package.h"
#include "Wrapper/Dx12Wrapper.h"

const int LOADING_WIDTH = 480;		//���[�h��ʂŕ\������A�C�R���̕�
const int LOADING_HEIGHT = 270;		//���[�h��ʂŕ\������A�C�R���̍���

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="dx12">Dx12Wrapper�C���X�^���X</param>
/// <param name="width">��ʕ�</param>
/// <param name="height">��ʍ���</param>
SpriteManager::SpriteManager(Dx12Wrapper& dx12, LONG width, LONG height):_dx12(dx12),_width(width),_height(height)
{
	CreateSpriteRS();																				//Sprite�p���[�g�V�O�l�`�����쐬
	InitSpriteDevices();																			//Sprite�p�I�u�W�F�N�g��������

	AdjustSpriteRect();

	_incrementSize =																				//�����̃T�C�Y���擾
		_dx12.Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	CreateUIBufferView(L"Asset/image/loading/1.png", "load_1");										//���[�h��ʂɉ�]����A�C�R��
	CreateUIBufferView(L"Asset/image/loading/2.png", "load_2");
	CreateUIBufferView(L"Asset/image/loading/3.png", "load_3");					
	CreateUIBufferView(L"Asset/image/loading/4.png", "load_4");					
	CreateUIBufferView(L"Asset/image/loading/5.png", "load_5");					
	CreateUIBufferView(L"Asset/image/loading/6.png", "load_6");					
	CreateUIBufferView(L"Asset/image/loading/7.png", "load_7");					
	CreateUIBufferView(L"Asset/image/loading/8.png", "load_8");
}

/// <summary>
/// SpriteBatch�����̃��[�g�V�O�l�`���E�V�F�[�_�[���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
SpriteManager::CreateSpriteRS()
{
	CD3DX12_DESCRIPTOR_RANGE spriteTblRange[2] = {};					//�f�B�X�N���v�^�����W(SRV�p)
	spriteTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	spriteTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	
	CD3DX12_ROOT_PARAMETER spriteRootParam[3] = {};						//���[�g�p�����[�^(SRV,CBV�p)
	spriteRootParam[0].InitAsDescriptorTable(
		1, &spriteTblRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootParam[1].InitAsConstantBufferView(						//[1]��ConstantBufferView�Ƃ��ď�����
		0, 0, D3D12_SHADER_VISIBILITY_ALL);
	spriteRootParam[2].InitAsDescriptorTable(
		1, &spriteTblRange[1], D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};						//�T���v���[
	samplerDesc.Init(0);												//������
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;		//�s�N�Z���V�F�[�_�[���猩����悤�ݒ�

	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};							//���[�g�V�O�l�`���쐬�p�\����
	rsDesc.Init(3,spriteRootParam,1,&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* rsBlob = nullptr;											//���[�g�V�O�l�`���̏�����
	ID3DBlob* errorBlob = nullptr;
	auto result = D3D12SerializeRootSignature(
		&rsDesc,
		D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rsBlob,
		&errorBlob);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	rsBlob->Release();													//�g��Ȃ��f�[�^���J��

	result = _dx12.Device()->CreateRootSignature(						//���[�g�V�O�l�`���쐬
		0,
		rsBlob->GetBufferPointer(),
		rsBlob->GetBufferSize(),
		IID_PPV_ARGS(_spriteRS.ReleaseAndGetAddressOf())
	);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	return result;
}

/// <summary>
/// Sprite�����I�u�W�F�N�g������������֐�
/// </summary>
void
SpriteManager::InitSpriteDevices()
{
	_gmemory = make_unique<GraphicsMemory>(_dx12.Device());								//�O���t�B�b�N�X�������̏�����

	ResourceUploadBatch resUploadBatch(_dx12.Device());									//�X�v���C�g�\���p�I�u�W�F�N�g�̏�����
	resUploadBatch.Begin();

	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);		//�����_�[�^�[�Q�b�g�X�e�[�g

	unique_ptr<CommonStates> _states = make_unique<CommonStates>(_dx12.Device());		//�T���v���[���擾���邽��State�I�u�W�F�N�g��������
	D3D12_GPU_DESCRIPTOR_HANDLE wrap = _states->AnisotropicWrap();						//GPU�n���h��

	SpriteBatchPipelineStateDescription psd(rtState, &CommonStates::NonPremultiplied);

	_spriteBatch = make_unique<SpriteBatch>(_dx12.Device(), resUploadBatch, psd);		//�X�v���C�g�\���p�I�u�W�F�N�g

	_batch = make_unique<PrimitiveBatch<VertexPositionColor>>(_dx12.Device());

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_SOLID,
		D3D12_CULL_MODE_NONE,false,D3D12_DEFAULT_DEPTH_BIAS,D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,true,false,true,
		0,D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF);

	EffectPipelineStateDescription ePsd
	(
		&VertexPositionColor::InputLayout,
		CommonStates::Opaque,
		CommonStates::DepthDefault,
		rastDesc,
		rtState,
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE
	);

	_effect = make_unique<BasicEffect>(_dx12.Device(), EffectFlags::VertexColor, ePsd);

	_world = SimpleMath::Matrix::Identity;
	_view = _dx12.ViewMatrix();
	_proj = _dx12.ProjMatrix();

	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//�t�H���g�\���p�I�u�W�F�N�g�̏�����		
	_dx12.CreateDescriptorHeap(_heapForSpriteFont,										//�t�H���g�E�摜�\���p�q�[�v
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,256, 
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	_tmpCPUHandle = _heapForSpriteFont->GetCPUDescriptorHandleForHeapStart();			//�q�[�v�n���h��(CPU)
	_tmpGPUHandle = _heapForSpriteFont->GetGPUDescriptorHandleForHeapStart();			//�q�[�v�n���h��(GPU)

	auto future = resUploadBatch.End(_dx12.CommandQueue());								//GPU���֓]��

	_dx12.WaitForCommandQueue();														//GPU���g�p�\�ɂȂ�܂őҋ@
	future.wait();

	_spriteBatch->SetViewport(*_dx12.ViewPort());										//�X�v���C�g�\���p�I�u�W�F�N�g���r���[�|�[�g�֓o�^

	return;
}

/// <summary>
/// �A�N�^�[�̓����蔻���`�悷��֐�
/// </summary>
/// <param name="collider">�����蔻��̃|�C���^</param>
void
SpriteManager::ColliderDraw(const shared_ptr<BoxCollider> collider)
{
	if (collider != nullptr)
	{
		auto v_d1 = VertexPositionColor(collider->Vertices()[0], { 0.f,1.0f,0.0f,.10f });
		auto v_d2 = VertexPositionColor(collider->Vertices()[1], { 0.f,1.0f,0.0f,.10f });
		auto v_d3 = VertexPositionColor(collider->Vertices()[2], { 0.f,1.0f,0.0f,.10f });
		auto v_d4 = VertexPositionColor(collider->Vertices()[3], { 0.f,1.0f,0.0f,.10f });

		_batch->DrawLine(v_d1, v_d2);
		_batch->DrawLine(v_d2, v_d3);
		_batch->DrawLine(v_d3, v_d4);
		_batch->DrawLine(v_d4, v_d1);

		auto v_u1 = VertexPositionColor(collider->Vertices()[4], { 0.f,1.0f,0.0f,.10f });
		auto v_u2 = VertexPositionColor(collider->Vertices()[5], { 0.f,1.0f,0.0f,.10f });
		auto v_u3 = VertexPositionColor(collider->Vertices()[6], { 0.f,1.0f,0.0f,.10f });
		auto v_u4 = VertexPositionColor(collider->Vertices()[7], { 0.f,1.0f,0.0f,.10f });

		_batch->DrawLine(v_u1, v_u2);
		_batch->DrawLine(v_u2, v_u3);
		_batch->DrawLine(v_u3, v_u4);
		_batch->DrawLine(v_u4, v_u1);

		_batch->DrawLine(v_d1, v_u1);
		_batch->DrawLine(v_d2, v_u2);
		_batch->DrawLine(v_d3, v_u3);
		_batch->DrawLine(v_d4, v_u4);
	}
}

/// <summary>
/// �摜�̃o�b�t�@�[�E�r���[���쐬����֐�
/// </summary>
/// <param name="path">�摜�̃p�X</param>
/// <param name="key">�A�z�z��̃L�[</param>
/// <returns>�����������������ǂ���</returns>
HRESULT
SpriteManager::CreateUIBufferView(const wchar_t* path,string key)
{
	TexMetadata meta = {};															//UI�摜�ǂݍ��ݗp�f�[�^
	ScratchImage scratch = {};

	auto ext = FileExtension(path);													//�g���q���擾

	auto result = _dx12._loadLambdaTable[ToString(ext)](path, &meta, scratch);		//�摜�f�[�^�̓ǂݍ���
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	auto img = scratch.GetImage(0, 0, 0);											//���f�[�^���擾

	DXGI_FORMAT format = meta.format;												//�t�H�[�}�b�g
	size_t width = meta.width;														//��
	size_t height = meta.height;													//����
	UINT16 arraySize = static_cast<UINT16>(meta.arraySize);							//�e�N�X�`���T�C�Y
	UINT16 mipLevels = static_cast<UINT16>(meta.mipLevels);		
	void* pixels = img->pixels;
	UINT rowPitch = static_cast<UINT>(img->rowPitch);
	UINT slicePitch = static_cast<UINT>(img->slicePitch);

	ID3D12Resource* tmpUIBuff = nullptr;											//�摜�f�[�^�������ݗp�o�b�t�@

	auto uiResDesc = CD3DX12_RESOURCE_DESC::Tex2D(									//���\�[�X�쐬�p�f�[�^
		format,
		(UINT)width,
		(UINT)height,
		arraySize,
		(UINT)mipLevels);

	result = _dx12.Device()->CreateCommittedResource(								//���\�[�X�쐬
		&_writeHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&uiResDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&tmpUIBuff));
	if (FAILED(result))
	{
		assert(0);
		return result;;
	}

	result = tmpUIBuff->WriteToSubresource(0,										//�摜������������
		nullptr,
		pixels,
		rowPitch,
		slicePitch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_tmpCPUHandle.ptr += _incrementSize;											//CPU�n���h�����Y����
	_tmpGPUHandle.ptr += _incrementSize;											//GPU�n���h�����Y����

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};									//UI�r���[�p�\���̂̍쐬
	srvDesc.Format = tmpUIBuff->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	_dx12.Device()->CreateShaderResourceView(tmpUIBuff, &srvDesc, _tmpCPUHandle);	//�r���[�쐬

	_GPUHandles[key] = _tmpGPUHandle;												//GPU�n���h����A�z�z��Ɋi�[

	return result;
}

/// <summary>
/// ��ʃT�C�Y�ύX���A��`�𒲐�����֐�
/// </summary>
void
SpriteManager::AdjustSpriteRect()
{
	_loadingRect = { LOADING_WIDTH,LOADING_HEIGHT,_width - LOADING_WIDTH,_height - LOADING_HEIGHT };	//���[�h��ʂ̐ݒ�

	AdjustWindowRect(&_loadingRect, WS_OVERLAPPEDWINDOW, false);		
}

/// <summary>
/// ���[�f�B���O��ʂł̉摜��`�悷��֐�
/// </summary>
void
SpriteManager::LoadingDraw()
{
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };					//�q�[�v���R�}���h���X�g�ɃZ�b�g
	_dx12.CommandList()->SetDescriptorHeaps(1, heap);

	_spriteBatch->Begin(_dx12.CommandList());										//�o�b�`���Z�b�g

	int count = ((clock() / static_cast<int>(60 * 1.4)) % 8) + 1;					//���݂̎��Ԃɉ����ĕ\������摜��ς��A���[�h�A�C�R���̃A�j���[�V������\��
	_spriteBatch->Draw(_GPUHandles["load_" + to_string(count)], XMUINT2(1, 1),
		_loadingRect, Colors::White);

	_spriteBatch->End();															//�o�b�`������
}

/// <summary>
/// �O���b�h��`�悷��֐�
/// </summary>
void
SpriteManager::GridDraw()
{
	_view = _dx12.ViewMatrix();											//�r���[�s����擾
	_proj = _dx12.ProjMatrix();											//�v���W�F�N�V�����s����擾

	_effect->SetWorld(_world);											//���[���h�E�r���[�E�v���W�F�N�V�����s����Z�b�g
	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	_effect->Apply(_dx12.CommandList());								//�R�}���h���X�g�ɃO���b�h��o�^

	_batch->Begin(_dx12.CommandList());

	VertexPositionColor v1(_origin - _XAxis, { 0.75f,0.0f,0.0f,.10f });		//X���̃O���b�h�̕`��
	VertexPositionColor v2(_origin + _XAxis, { 0.75f,0.0f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	v1 = VertexPositionColor(_origin - _YAxis, { 0.f,0.75f,0.0f,.10f });	//Y���̃O���b�h�̕`��
	v2 = VertexPositionColor(_origin + _YAxis, { 0.f,0.75f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	v1 = VertexPositionColor(_origin - _ZAxis, { 0.0f,0.0f,0.75f,.10f });	//X���̃O���b�h�̕`��
	v2 = VertexPositionColor(_origin + _ZAxis, { 0.0f,0.0f,0.75f,.10f });
	_batch->DrawLine(v1, v2);

	for (const auto& actor : _actorAndObjs)
	{
		if (actor != nullptr)ColliderDraw(actor->Collider());
	}

	_batch->End();
}

/// <summary>
/// �O���t�B�b�N�X���������R�}���h�L���[�ɃZ�b�g����֐�
/// </summary>
void
SpriteManager::Commit()
{
	_gmemory->Commit(_dx12.CommandQueue());
}

/// <summary>
/// �A�N�^�[��ݒ肷��֐�
/// </summary>
/// <param name="actor">�A�N�^�[</param>
void
SpriteManager::SetActors(vector<shared_ptr<FBXBase>> actorAndObjects)
{
	_actorAndObjs = actorAndObjects;
}