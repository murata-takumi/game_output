#include "Functions.h"
#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/BoxCollider2D.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/FbxActor.h"
#include "Manager/InputManager.h"
#include "Manager/SpriteManager.h"
#include "Package/Package.h"
#include "Wrapper/Dx12Wrapper.h"

const int LOADING_WIDTH = 480;		//���[�h��ʂŕ\������A�C�R���̕�
const int LOADING_HEIGHT = 270;		//���[�h��ʂŕ\������A�C�R���̍���

/// <summary>
/// �V���O���g����Ԃ�
/// </summary>
/// <returns>�V���O���g��</returns>
SpriteManager&
SpriteManager::Instance()
{
	static SpriteManager instance;
	return instance;
}

/// <summary>
/// �������֐�
/// </summary>
/// <param name="width">�E�B���h�E��</param>
/// <param name="height">�E�B���h�E����</param>
void
SpriteManager::Init(LONG width, LONG height)
{
	_width = width;
	_height = height;

	//Sprite�p���[�g�V�O�l�`�����쐬
	CreateSpriteRS();

	//Sprite�p�I�u�W�F�N�g��������
	InitSpriteDevices();

	_startColl = make_unique<BoxCollider2D>(XMVectorSet(240, 60, 0, 0), XMVectorSet(610, 535, 0, 0));
	_exitColl = make_unique<BoxCollider2D>(XMVectorSet(240, 60, 0, 0), XMVectorSet(610, 635, 0, 0));

	//���[�h��ʗp��`�𒲐�
	AdjustSpriteRect();

	//�q�[�v�n���h���̍����̃T�C�Y���擾
	_incrementSize =
		Dx12Wrapper::Instance().Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//���[�h��ʂɉ�]����A�C�R��
	CreateUIBufferView(L"Asset/image/loading/1.png", "load_1");
	CreateUIBufferView(L"Asset/image/loading/2.png", "load_2");
	CreateUIBufferView(L"Asset/image/loading/3.png", "load_3");
	CreateUIBufferView(L"Asset/image/loading/4.png", "load_4");
	CreateUIBufferView(L"Asset/image/loading/5.png", "load_5");
	CreateUIBufferView(L"Asset/image/loading/6.png", "load_6");
	CreateUIBufferView(L"Asset/image/loading/7.png", "load_7");
	CreateUIBufferView(L"Asset/image/loading/8.png", "load_8");

	CreateUIBufferView(L"Asset/image/button.png", "start");
	CreateUIBufferView(L"Asset/image/button.png", "exit");

	CreateUIBufferView(L"Asset/image/background.png", "background");
}

/// <summary>
/// SpriteBatch�����̃��[�g�V�O�l�`���E�V�F�[�_�[���쐬����֐�
/// </summary>
/// <returns>�����������������ǂ���</returns>
HRESULT
SpriteManager::CreateSpriteRS()
{
	//�f�B�X�N���v�^�����W(SRV�p)
	CD3DX12_DESCRIPTOR_RANGE spriteTblRange[2] = {};					
	spriteTblRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	spriteTblRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

	
	//���[�g�p�����[�^(SRV,CBV�p)
	CD3DX12_ROOT_PARAMETER spriteRootParam[3] = {};						
	spriteRootParam[0].InitAsDescriptorTable(1, &spriteTblRange[0], D3D12_SHADER_VISIBILITY_PIXEL);
	spriteRootParam[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	spriteRootParam[2].InitAsDescriptorTable(1, &spriteTblRange[1], D3D12_SHADER_VISIBILITY_ALL);

	//�T���v���[
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = {};						
	samplerDesc.Init(0);
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	//���[�g�V�O�l�`���̏�����
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc = {};							
	rsDesc.Init(3,spriteRootParam,1,&samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ID3DBlob* rsBlob = nullptr;											
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
	//�g��Ȃ��f�[�^���J��
	rsBlob->Release();							

	//���[�g�V�O�l�`���쐬
	result = Dx12Wrapper::Instance().Device()->CreateRootSignature(						
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
	//�O���t�B�b�N�X�������̏�����
	_gmemory = make_unique<GraphicsMemory>(Dx12Wrapper::Instance().Device());								

	//�X�v���C�g�\���p�I�u�W�F�N�g�̏�����
	ResourceUploadBatch resUploadBatch(Dx12Wrapper::Instance().Device());									
	resUploadBatch.Begin();

	//�����_�[�^�[�Q�b�g�X�e�[�g
	RenderTargetState rtState(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT);		

	//�T���v���[���擾���邽��State�I�u�W�F�N�g��������
	unique_ptr<CommonStates> _states = make_unique<CommonStates>(Dx12Wrapper::Instance().Device());		

	//GPU�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE wrap = _states->AnisotropicWrap();						

	SpriteBatchPipelineStateDescription psd(rtState, &CommonStates::NonPremultiplied);

	//�X�v���C�g�\���p�I�u�W�F�N�g
	_spriteBatch = make_unique<SpriteBatch>(Dx12Wrapper::Instance().Device(), resUploadBatch, psd);		

	_batch = make_unique<PrimitiveBatch<VertexPositionColor>>(Dx12Wrapper::Instance().Device());

	CD3DX12_RASTERIZER_DESC rastDesc(D3D12_FILL_MODE_WIREFRAME,
		D3D12_CULL_MODE_BACK,false,D3D12_DEFAULT_DEPTH_BIAS,D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
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

	_effect = make_unique<BasicEffect>(Dx12Wrapper::Instance().Device(), EffectFlags::VertexColor, ePsd);

	_world = SimpleMath::Matrix::Identity;
	_view = Dx12Wrapper::Instance().ViewMatrix();
	_proj = Dx12Wrapper::Instance().ProjMatrix();

	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//�t�H���g�\���p�I�u�W�F�N�g�̏�����		
	Dx12Wrapper::Instance().CreateDescriptorHeap(_heapForSpriteFont,
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,1,256, 
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	_tmpCPUHandle = _heapForSpriteFont->GetCPUDescriptorHandleForHeapStart();
	_tmpGPUHandle = _heapForSpriteFont->GetGPUDescriptorHandleForHeapStart();

	_spriteFont = make_unique<SpriteFont>(Dx12Wrapper::Instance().Device(),
		resUploadBatch,
		L"Asset/font/fonttest.spritefont",
		_tmpCPUHandle,
		_tmpGPUHandle);

	//�`�施�߂�GPU���֓]��
	auto future = resUploadBatch.End(Dx12Wrapper::Instance().CommandQueue());								

	//GPU���g�p�\�ɂȂ�܂őҋ@
	Dx12Wrapper::Instance().WaitForCommandQueue();														
	future.wait();

	//�X�v���C�g�\���p�I�u�W�F�N�g���r���[�|�[�g�֓o�^
	_spriteBatch->SetViewport(*Dx12Wrapper::Instance().ViewPort());										

	return;
}

/// <summary>
/// �A�N�^�[�̓����蔻���`�悷��֐�
/// </summary>
/// <param name="collider">�����蔻��̃|�C���^</param>
void
SpriteManager::ColliderDraw(const shared_ptr<ICollider> collider)
{
	//�q�[�v���R�}���h���X�g�ɃZ�b�g
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	auto transMat = XMMatrixTranslationFromVector(
		RightPosToLeftPos(Vector3(
			collider->Center()->X(),
			collider->Center()->Y(),
			collider->Center()->Z())));

	//�R�}���h���X�g�ɃO���b�h��o�^
	_effect->SetWorld(transMat);
	_effect->Apply(Dx12Wrapper::Instance().CommandList());

	if (dynamic_pointer_cast<BoxCollider>(collider))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(collider);
		auto primitive = GeometricPrimitive::CreateBox(
		Vector3(
			tempBox->HalfLength().X() * 2,
			tempBox->HalfLength().Y() * 2,
			tempBox->HalfLength().Z() * 2));
		primitive->Draw(Dx12Wrapper::Instance().CommandList());
	}
	else if (dynamic_pointer_cast<SphereCollider>(collider))
	{
		auto radius = dynamic_pointer_cast<SphereCollider>(collider)->Radius();
		auto primitive = GeometricPrimitive::CreateSphere(radius, 8);
		primitive->Draw(Dx12Wrapper::Instance().CommandList());
	}

	//���ɖ߂�
	_effect->SetWorld(_world);
	_effect->Apply(Dx12Wrapper::Instance().CommandList());
}

/// <summary>
/// �}�E�X�J�[�\������`���ɂ��邩�ǂ����m�F����
/// </summary>
/// <param name="rect">�Ώۂ̋�`</param>
/// <returns>��`���ɂ��邩�ǂ���</returns>
bool
SpriteManager::IsCursorInRect(RECT rect)
{
	auto cursorPos = InputManager::Instance().MousePos();

	if (cursorPos._posX < rect.left)return false;
	if (rect.right < cursorPos._posX)return false;
	if (cursorPos._posY < rect.top)return false;
	if (rect.bottom < cursorPos._posY)return false;

	return true;
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
	//UI�摜�ǂݍ��ݗp�f�[�^
	TexMetadata meta = {};															
	ScratchImage scratch = {};

	//�摜�f�[�^�̓ǂݍ���
	auto ext = FileExtension(path);			
	auto result = Dx12Wrapper::Instance()._loadLambdaTable[ToString(ext)](path, &meta, scratch);		
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	//�f�[�^���擾
	auto img = scratch.GetImage(0, 0, 0);	
	DXGI_FORMAT format = meta.format;
	size_t width = meta.width;
	size_t height = meta.height;
	UINT16 arraySize = static_cast<UINT16>(meta.arraySize);
	UINT16 mipLevels = static_cast<UINT16>(meta.mipLevels);		
	void* pixels = img->pixels;
	UINT rowPitch = static_cast<UINT>(img->rowPitch);
	UINT slicePitch = static_cast<UINT>(img->slicePitch);

	//�摜�f�[�^�������ݗp�o�b�t�@
	ID3D12Resource* tmpUIBuff = nullptr;											

	//���\�[�X�쐬
	auto uiResDesc = CD3DX12_RESOURCE_DESC::Tex2D(									
		format,
		(UINT)width,
		(UINT)height,
		arraySize,
		(UINT)mipLevels);
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource(								
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

	//�摜������������
	result = tmpUIBuff->WriteToSubresource(0,										
		nullptr,
		pixels,
		rowPitch,
		slicePitch);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	//CPU�GPU�n���h�����Y����
	_tmpCPUHandle.ptr += _incrementSize;											
	_tmpGPUHandle.ptr += _incrementSize;

	//UI�r���[�̍쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};									
	srvDesc.Format = tmpUIBuff->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Dx12Wrapper::Instance().Device()->CreateShaderResourceView(tmpUIBuff, &srvDesc, _tmpCPUHandle);

	//GPU�n���h����A�z�z��Ɋi�[
	_GPUHandles[key] = _tmpGPUHandle;												

	return result;
}

/// <summary>
/// �E����W��������W�ɕϊ�����֐�
/// </summary>
/// <param name="rightPos">�E����W</param>
/// <returns>������W</returns>
XMVECTOR
SpriteManager::RightPosToLeftPos(const Vector3& rightPos)
{
	return XMVectorSet(rightPos.X() * -1, rightPos.Y(), rightPos.Z(), 0.0f);
}

/// <summary>
/// ��ʃT�C�Y�ύX���A��`�𒲐�����֐�
/// </summary>
void
SpriteManager::AdjustSpriteRect()
{
	//���[�h��ʂ̐ݒ�
	_loadingRect = { LOADING_WIDTH,LOADING_HEIGHT,_width - LOADING_WIDTH,_height - LOADING_HEIGHT };
	//�w�i�p��`�̐ݒ�
	_BGRect = *Dx12Wrapper::Instance().Rect();

	auto startRect = _startColl->Rect();
	auto exitRect = _exitColl->Rect();

	AdjustWindowRect(&_loadingRect, WS_OVERLAPPEDWINDOW, false);		
	AdjustWindowRect(&startRect, WS_OVERLAPPEDWINDOW, false);
	AdjustWindowRect(&exitRect, WS_OVERLAPPEDWINDOW, false);
	AdjustWindowRect(&_BGRect, WS_OVERLAPPEDWINDOW, false);		
}

/// <summary>
/// ���[�f�B���O��ʂł̉摜��`�悷��֐�
/// </summary>
void
SpriteManager::LoadingDraw()
{
	//�q�[�v���R�}���h���X�g�ɃZ�b�g
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };					
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//�o�b�`���Z�b�g
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());										

	//���݂̎��Ԃɉ����ĕ\������摜��ς��A���[�h�A�C�R���̃A�j���[�V������\��
	int count = ((clock() / static_cast<int>(60 * 1.4)) % 8) + 1;					
	_spriteBatch->Draw(_GPUHandles["load_" + to_string(count)], XMUINT2(1, 1),
		_loadingRect, Colors::White);

	//�o�b�`������
	_spriteBatch->End();															
}

/// <summary>
/// �^�C�g����ʂł̃{�^���̕`��
/// </summary>
void
SpriteManager::ButtonDraw()
{
	//�q�[�v���R�}���h���X�g�ɃZ�b�g
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//�o�b�`���Z�b�g
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());

	//�w�i�p�摜�r���[���Z�b�g
	_spriteBatch->Draw(_GPUHandles["start"], XMUINT2(1, 1),
		_startColl->Rect(), (TitleIsOnStart() ? Colors::White : Colors::LightGray));

	//�w�i�p�摜�r���[���Z�b�g
	_spriteBatch->Draw(_GPUHandles["exit"], XMUINT2(1, 1),
		_exitColl->Rect(), (TitleIsOnExit() ? Colors::White : Colors::LightGray));

	string start = "Start";
	string exit = "Exit";

	_spriteFont->DrawString(_spriteBatch.get(),start.c_str(),_startColl->StrCenterPos(start), DirectX::Colors::GhostWhite);
	_spriteFont->DrawString(_spriteBatch.get(), exit.c_str(),_exitColl->StrCenterPos(exit), DirectX::Colors::GhostWhite);

	//�o�b�`������
	_spriteBatch->End();
}

/// <summary>
/// �w�i��`�悷��֐�
/// </summary>
void
SpriteManager::BackGroundDraw()
{
	//�q�[�v���R�}���h���X�g�ɃZ�b�g
	ID3D12DescriptorHeap* heap[] = { _heapForSpriteFont.Get() };
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);

	//�o�b�`���Z�b�g
	_spriteBatch->Begin(Dx12Wrapper::Instance().CommandList());

	//�w�i�p�摜�r���[���Z�b�g
	_spriteBatch->Draw(_GPUHandles["background"], XMUINT2(1, 1),
		_BGRect, Colors::White);

	//�o�b�`������
	_spriteBatch->End();
}

/// <summary>
/// �O���b�h��`�悷��֐�
/// </summary>
void
SpriteManager::GridDraw(const map<string, shared_ptr<IFbx>> actorAndObjs)
{
	//�r���[��v���W�F�N�V�����s����擾
	_view = Dx12Wrapper::Instance().ViewMatrix();											
	_proj = Dx12Wrapper::Instance().ProjMatrix();

	//���[���h�E�r���[�E�v���W�F�N�V�����s����Z�b�g
	_effect->SetWorld(_world);											
	_effect->SetView(_view);
	_effect->SetProjection(_proj);

	//�R�}���h���X�g�ɃO���b�h��o�^
	_effect->Apply(Dx12Wrapper::Instance().CommandList());								

	//�`��J�n
	_batch->Begin(Dx12Wrapper::Instance().CommandList());

	//X���̃O���b�h�̕`��
	VertexPositionColor v1(_origin - _XAxis, { 0.75f,0.0f,0.0f,.10f });		
	VertexPositionColor v2(_origin + _XAxis, { 0.75f,0.0f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	//Y���̃O���b�h�̕`��
	v1 = VertexPositionColor(_origin - _YAxis, { 0.f,0.75f,0.0f,.10f });	
	v2 = VertexPositionColor(_origin + _YAxis, { 0.f,0.75f,0.0f,.10f });
	_batch->DrawLine(v1, v2);

	//X���̃O���b�h�̕`��
	v1 = VertexPositionColor(_origin - _ZAxis, { 0.0f,0.0f,0.75f,.10f });	
	v2 = VertexPositionColor(_origin + _ZAxis, { 0.0f,0.0f,0.75f,.10f });
	_batch->DrawLine(v1, v2);

	//�����蔻������I�u�W�F�N�g�����݂����瓖���蔻��̕`��
	for (const auto& actor : actorAndObjs)
	{
		if (actor.second != nullptr)
		{
			if (dynamic_pointer_cast<FbxActor>(actor.second))
			{
				ColliderDraw(dynamic_pointer_cast<FbxActor>(actor.second)->GetColForGround());
			}

			ColliderDraw(actor.second->Collider());
			//X���̃O���b�h�̕`��
			v1 = VertexPositionColor(RightPosToLeftPos(actor.second->CurrentPosition()), {1.0f,0.0f,0.0f,.10f});
			v2 = VertexPositionColor(RightPosToLeftPos(*actor.second->Collider()->Center()), {1.0f,0.0f,0.0f,.10f});
			_batch->DrawLine(v1, v2);
		}
	}

	//�`��I��
	_batch->End();
}

/// <summary>
/// �O���t�B�b�N�X���������R�}���h�L���[�ɃZ�b�g����֐�
/// </summary>
void
SpriteManager::Commit()
{
	_gmemory->Commit(Dx12Wrapper::Instance().CommandQueue());
}

/// <summary>
/// �X�^�[�g�{�^����������Ă��邩����
/// </summary>
/// <returns>�X�^�[�g�{�^����ɃJ�[�\�������邩</returns>
bool
SpriteManager::TitleIsOnStart()
{
	return IsCursorInRect(_startColl->Rect());
}

/// <summary>
/// �I���{�^����������Ă��邩����
/// </summary>
/// <returns>�I���{�^����ɃJ�[�\�������邩</returns>
bool
SpriteManager::TitleIsOnExit()
{
	return IsCursorInRect(_exitColl->Rect());
}