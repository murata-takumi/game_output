#include "Manager/EffectManager.h"
#include "Manager/ImGuiManager.h"
#include "Manager/SpriteManager.h"
#include "Renderer/PeraRenderer.h"
#include "Renderer/Renderer.h"
#include "Wrapper/Dx12Wrapper.h"
#include "Wrapper/SphericalCoordinates.h"
						
//���_�������_�̃x�N�g����XZ���ʂɐ������邽�߂̃x�N�g��
const XMVECTOR XZ_PLANE = XMVectorSet(1.0f,0.0f,1.0f,0.0f);	
//�t�F�[�h�C���E�A�E�g�ɂ����鎞��
const float FADE_TIME = 1.0f;
//���a�̏����l
const int INIT_RAD = 300;

/// <summary>
/// �f�o�b�O�p���C���[������������֐�
/// </summary>
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	auto result = D3D12GetDebugInterface(
		IID_PPV_ARGS(&debugLayer));
		
	debugLayer->EnableDebugLayer();				//�f�o�b�O���C���[��L��������
	debugLayer->Release();						//�L����������C���^�[�t�F�C�X���J������
}

/// <summary>
/// �f�o�C�X�֘A������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::InitializeDXGIDevice()
{
	//Debug�������łȂ����Ńt�@�N�g���[�̍쐬�֐���ς���
#ifdef _DEBUG
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(_dxgiFactory.ReleaseAndGetAddressOf()));
#else
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif // _DEBUG
	
	D3D_FEATURE_LEVEL levels[] = {												//�t�B�[�`���[���x���̔z���������
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	vector<IDXGIAdapter*> adapters;												//�񋓂����A�_�v�^�[���i�[
	IDXGIAdapter* tmpAdapter = nullptr;											//�A�_�v�^�[

	for (int i = 0;
		_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;++i	//�t�@�N�g���[���̑S�A�_�v�^�[���x�N�g���Ɋi�[
		)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)													//�i�[�����S�A�_�v�^�[�ɑ΂����[�v
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);													//�A�_�v�^�[�̐������L�q����\���̂��擾

		wstring strDesc = adesc.Description;									//�A�_�v�^�[�̖��O���擾

		if (strDesc.find(L"NVIDIA") != string::npos)							//�A�_�v�^�[�̖��O������̖��O�ƈ�v�����烋�[�v�I��
		{
			tmpAdapter = adpt;													//�f�o�C�X�쐬�Ɏg�p����A�_�v�^�[������
			break;
		}
	}

	for (auto lv : levels)														//�����������t�B�[�`���[���x���ɑ΂����[�v
	{
		if (D3D12CreateDevice(tmpAdapter, lv, IID_PPV_ARGS(&_dev)) == S_OK)		//�f�o�C�X���쐬�ł���t�B�[�`���[���x����T��
		{
			result = S_OK;
			break;																//�����\�ȃo�[�W���������������烋�[�v�I��
		}
	}
	return result;
}

/// <summary>
/// �R�}���h�֘A������������֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::InitializeCommand()
{
	auto result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,	//�R�}���h�A���P�[�^���쐬
		IID_PPV_ARGS(_cmdAllocator.ReleaseAndGetAddressOf()));

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,			//�R�}���h���X�g���쐬
		_cmdAllocator.Get(), nullptr,
		IID_PPV_ARGS(_cmdList.ReleaseAndGetAddressOf()));

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};									//�R�}���h�L���[�ݒ�\���̂��쐬�E�ݒ�
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = _dev->CreateCommandQueue(&cmdQueueDesc,							//�R�}���h�L���[���쐬
		IID_PPV_ARGS(_cmdQueue.ReleaseAndGetAddressOf()));

	return result;
}

/// <summary>
/// �X���b�v�`�F�[�����쐬����֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E���ʗp�f�[�^</param>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateSwapChain(const HWND& hwnd)
{
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};						//�X���b�v�`�F�[���ݒ�p�\���́E�ݒ�
	swapchainDesc.Width = _winSize.cx;
	swapchainDesc.Height = _winSize.cy;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.BufferCount = 2;
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;				
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = _dxgiFactory->CreateSwapChainForHwnd(				//�X���b�v�`�F�[���쐬
		_cmdQueue.Get(),
		hwnd,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)_swapchain.ReleaseAndGetAddressOf());

	return result;
}

/// <summary>
/// �����_�[�^�[�Q�b�g���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT 
Dx12Wrapper::CreateRenderTargetsView()
{
	auto result = CreateDescriptorHeap(_rtvHeap,
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV,0,2,
		D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	DXGI_SWAP_CHAIN_DESC swcDesc = {};									//�X���b�v�`�F�[���̏��擾
	result = _swapchain->GetDesc(&swcDesc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_backBuffers.resize(swcDesc.BufferCount);							//�o�b�N�o�b�t�@�[�̐����X���b�v�`�F�[���̃o�b�t�@�[���ɍ��킹��
		
	D3D12_CPU_DESCRIPTOR_HANDLE handle =								//�f�B�X�N���v�^�q�[�v�̐擪�A�h���X(�n���h��)
		_rtvHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};							//�����_�[�^�[�Q�b�g�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int idx = 0; idx < (int)swcDesc.BufferCount; ++idx)			//�e�o�b�N�o�b�t�@�[�ɑ΂����[�v����
	{
		result = _swapchain->GetBuffer(													//�X���b�v�`�F�[������f�[�^���擾�A�o�b�N�o�b�t�@�[�Ɋ��蓖�Ă�
			idx, IID_PPV_ARGS(&_backBuffers[idx]));	
		if (FAILED(result))
		{
			assert(0);
			return result;
		}

		_dev->CreateRenderTargetView(													//�o�b�N�o�b�t�@�[��Ώۂ�RTV���쐬
			_backBuffers[idx],
			&rtvDesc,
			handle);

		handle.ptr +=																	//�n���h���̈ʒu�����炷
			_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	DXGI_SWAP_CHAIN_DESC1 desc = {};													//�X���b�v�`�F�[�������擾�i��L��swcDesc�Ƃ͕ʁj
	result = _swapchain->GetDesc1(&desc);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	_viewPort.reset(new CD3DX12_VIEWPORT(_backBuffers[0]));								//�X���b�v�`�F�[���̏������Ƀr���[�|�[�g��������
	_rect.reset(new CD3DX12_RECT(0, 0, desc.Width, desc.Height));						//�������V�U�[��`��������

	return result;
}

/// <summary>
/// �r���[�v���W�F�N�V�����p�r���[���쐬
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT 
Dx12Wrapper::CreateSceneView()
{
	auto sceneResDesc = 
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(SceneData) + _constSize) & ~_constSize);

	CreateResource(_sceneConstBuff,_uploadHeapProp,										//�萔�o�b�t�@�[�쐬
		sceneResDesc,D3D12_RESOURCE_STATE_GENERIC_READ);

	_mappedScene = nullptr;																//�萔�o�b�t�@�[�ɑ΂��������߂�悤�ɂ���
	auto result = _sceneConstBuff->Map(0,nullptr,(void**)&_mappedScene);

	result = CreateDescriptorHeap(_sceneDescHeap,										//�f�B�X�N���v�^�q�[�v�쐬
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,0,1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//�萔�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	cbvDesc.BufferLocation = _sceneConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_sceneConstBuff->GetDesc().Width;
	
	_dev->CreateConstantBufferView(														//�萔�o�b�t�@�[�r���[�̍쐬
		&cbvDesc,
		_sceneDescHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �[�x�X�e���V���r���[���쐬���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateDepthStencilView()
{
	D3D12_HEAP_PROPERTIES depthHeapProp = {};											//�[�x�X�e���V���o�b�t�@�[�p�q�[�v�v���p�e�B���쐬
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	depthHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	depthHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC depthResDesc = {};												//�o�b�t�@�[�p���\�[�X�f�B�X�N���쐬�E�ݒ�
	depthResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResDesc.Width = _winSize.cx;
	depthResDesc.Height = _winSize.cy;
	depthResDesc.DepthOrArraySize = 1;
	depthResDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthResDesc.SampleDesc.Count = 1;
	depthResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE depthClearValue = {};												//�N���A�o�����[�̍쐬�E�ݒ�
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;

	auto result = _dev->CreateCommittedResource(										//�o�b�t�@�[�̍쐬
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(_depthBuffer.ReleaseAndGetAddressOf()));

	result = CreateDescriptorHeap(_dsvHeap,
		D3D12_DESCRIPTOR_HEAP_TYPE_DSV,0,1, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};											//�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

	
	_dev->CreateDepthStencilView(														//�r���[�̍쐬
		_depthBuffer.Get(),&dsvDesc,_dsvHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �G�t�F�N�g�K�p�����߂�f�[�^�p�̃q�[�v�E�o�b�t�@�[���쐬����֐�
/// </summary>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateFactorBufferAndView()
{
	auto factorResDesc
		= CD3DX12_RESOURCE_DESC::Buffer((sizeof(Factor) + _constSize) & ~_constSize);	//���\�[�X�f�B�X�N�쐬

	auto result = CreateResource(_factorConstBuff,_writeHeapProp,						//�o�b�t�@�[�쐬
		factorResDesc,D3D12_RESOURCE_STATE_GENERIC_READ);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	result = _factorConstBuff->Map(0, nullptr, (void**)&_mappedFactor);					//�o�b�t�@�[�֏������߂�悤�ɂ���
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	UpdateFade();

	result = CreateDescriptorHeap(_factorCBVHeap,										//�f�B�X�N���v�^�q�[�v�쐬
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,0,1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
	if (FAILED(result))
	{
		assert(0);
		return result;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};										//�萔�o�b�t�@�[�r���[�ݒ�p�\���̂̍쐬�E�ݒ�
	cbvDesc.BufferLocation = _factorConstBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)_factorConstBuff->GetDesc().Width;

	_dev->CreateConstantBufferView(														//�萔�o�b�t�@�[�r���[�̍쐬
		&cbvDesc,
		_factorCBVHeap->GetCPUDescriptorHandleForHeapStart());

	return result;
}

/// <summary>
/// �e�N�X�`�����[�h�p�e�[�u�����쐬����֐�
/// �t�@�C�����Ɋ܂܂�Ă���g���q�ɉ����Ď��s����֐���ς���
/// </summary>
void
Dx12Wrapper::CreateTextureLoaderTable()
{
	_loadLambdaTable["sph"]													//sph,spa,png,jpg�g���q��key�ɁALoadFromWICFile�֐���value�ɂ���
		= _loadLambdaTable["spa"]
		= _loadLambdaTable["bmp"]
		= _loadLambdaTable["png"]
		= _loadLambdaTable["jpg"]
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromWICFile(path.c_str(), 0, meta, img);
	};

	_loadLambdaTable["tga"]													//tga�g���q��key�ɁALoadFromTGAFile�֐���value�ɂ���
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromTGAFile(path.c_str(), meta, img);
	};

	_loadLambdaTable["dds"]													//dds�g���q��key�ɁALoadFromDDSFile�֐���value�ɂ���
		= [](const wstring& path, TexMetadata* meta, ScratchImage& img)
		->HRESULT
	{
		return LoadFromDDSFile(path.c_str(), 0, meta, img);
	};
}

/// <summary>
/// ���_�A�����_�̃x�N�g��������������֐�
/// </summary>
void
Dx12Wrapper::InitVector()
{
	_eye = _initEye;
	_target = _initTarget;
}

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="deltaTime">1�t���[��������̕b��</param>
Dx12Wrapper::Dx12Wrapper() :
	 _perspective(true), _up(0, 1, 0),_light(0.0f,1.0f,0.0f), _fade(1.0f),_initEye(0, 50, INIT_RAD),_initTarget(0, 50, 0)
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Dx12Wrapper::~Dx12Wrapper()
{

}

/// <summary>
/// �V���O���g����Ԃ��֐�
/// </summary>
/// <returns>�V���O���g��</returns>
Dx12Wrapper&
Dx12Wrapper::Instance()
{
	static Dx12Wrapper instance;
	return instance;
}

/// <summary>
/// �������֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="deltaTime">�t���[���Ԃ̌o�ߎ���</param>
void
Dx12Wrapper::Init(HWND hwnd, float deltaTime)
{
#ifdef _DEBUG
	//�f�o�b�O�p���C���[���N��
	EnableDebugLayer();													
#endif 
	//�E�B���h�E�̃T�C�Y���擾
	_winSize = Application::Instance().GetWindowSize();
	//�t���[�����[�g���擾
	_rate = Application::Instance().GetRate();
	//�t���[���Ԃ̎��Ԃ̍������擾
	_deltaTime = deltaTime;

	//�f�o�C�X�֘A��������
	InitializeDXGIDevice();

	//�R�}���h�֘A��������
	InitializeCommand();

	CreateSwapChain(hwnd);												//�X���b�v�`�F�[�����쐬

	CreateRenderTargetsView();											//�����_�[�^�[�Q�b�g���쐬

	InitVector();														//���_�A�����_�̃x�N�g����������

	CreateSceneView();													//�r���[�v���W�F�N�V�����p�r���[���쐬

	CreateTextureLoaderTable();											//�e�N�X�`�����[�h�p�e�[�u�����쐬

	CreateDepthStencilView();											//�[�x�X�e���V���r���[���쐬

	CreateFactorBufferAndView();										//�t�F�[�h�C���^�A�E�g�p�o�b�t�@�[�E�r���[���쐬

	_dev->CreateFence(													//�t�F���X���쐬
		_fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));

	//_effect.reset(new EffectManager(*this));							//EffectManager�C���X�^���X��������
}

/// <summary>
/// ���\�[�X��J�ڂ�����֐�
/// </summary>
/// <param name="resource">�J�ڂ����������\�[�X</param>
/// <param name="before">�J�ڑO�̃X�e�[�g</param>
/// <param name="after">�J�ڌ�̃X�e�[�g</param>
void
Dx12Wrapper::BarrierTransition(
	ID3D12Resource* resource,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after)
{
	_barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before, after);	//�ϐ���ݒ�

	//���\�[�X��J�ڂ�����
	_cmdList->ResourceBarrier(1, &_barrier);
}

/// <summary>
/// XZ���ʂɐ����������_���W���璍���_���W�ւ̃x�N�g�����擾����֐�
/// </summary>
/// <returns>�x�N�g��</returns>
XMVECTOR
Dx12Wrapper::GetXZVecEyeToTarget()const
{
	XMVECTOR vec = XMVectorSet
	(_target.x - _eye.x, _target.y - _eye.y, _target.z - _eye.z, 0);	//�܂����_���W���璍���_���W�ւ̃x�N�g�����擾

	vec = XMVectorMultiply(vec, XZ_PLANE);								//Y������0�ɂ��A�x�N�g����XZ���ʂɐ���

	XMVECTOR ret = XMVector3Normalize(vec);								//�x�N�g���𐳋K��
	
	return ret;
}

/// <summary>
/// ���_�������_�̃x�N�g���ɑ΂��E�ɐ����ȃx�N�g����Ԃ��֐�
/// </summary>
/// <returns>�����x�N�g��</returns>
XMVECTOR
Dx12Wrapper::GetRightVector()const
{
	XMVECTOR eyeToTrgtVec = GetXZVecEyeToTarget();						//���_�������_�̃x�N�g�����擾
	XMVECTOR upVec = XMVectorSet(_up.x, _up.y, _up.z, 0.0f);			//��x�N�g�����擾

	XMVECTOR cross = XMVector3Cross(eyeToTrgtVec, upVec);				//2�x�N�g���̊O�ς����߂�

	return cross;
}

/// <summary>
/// �t���[���Ԃ̌o�ߎ��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�t���[���Ԃ̌o�ߎ���</returns>
float 
Dx12Wrapper::GetDeltaTime()const
{
	return _deltaTime;
}

/// <summary>
/// �J�������ߕt����E��������֐�
/// </summary>
/// <param name="x">�ړ�����</param>
void
Dx12Wrapper::ScalingCoordinates(int x)
{
	_eye = XMFLOAT3
	(
		_coordinates->Scaling(x * _deltaTime).ToCartesian().x + _target.x,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().y + _target.y,
		_coordinates->Scaling(x * _deltaTime).ToCartesian().z + _target.z
	);
}

/// <summary>
/// �J�����𕽍s�ړ�������֐�
/// </summary>
/// <param name="dir">���s�ړ����������</param>
/// <param name="value">�ړ�����</param>
void
Dx12Wrapper::TranslateCoordinates(XMVECTOR vec)
{
	_target.x += vec.m128_f32[0] * _deltaTime * 300.0f;
	_target.y += vec.m128_f32[1] * _deltaTime * 300.0f;
	_target.z += vec.m128_f32[2] * _deltaTime * 300.0f;

	//���̌�J�������W���X�V
	_eye =
		XMFLOAT3
		(
			_coordinates->ToCartesian().x + _target.x,
			_coordinates->ToCartesian().y + _target.y,
			_coordinates->ToCartesian().z + _target.z
		);
}

/// <summary>
/// SphericalCoordinates��̃J�������W����]������֐�
/// </summary>
/// <param name="deg">���ʊp���p�����߂�</param>
/// <param name="value">�ォ�����A�E������</param>
void
Dx12Wrapper::RotateCoordinates(Degree deg, float value)
{
	switch (deg)
	{
	case Degree::Azimth:	//���ʊp�̕����ɉ�]������
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().x + _target.x,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().y + _target.y,
			_coordinates->Rotate(value * _deltaTime, 0.0f).ToCartesian().z + _target.z
		);
		break;
	case Degree::Elevation:	//�p�̕����ɉ�]������
		_eye = XMFLOAT3
		(
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().x + _target.x,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().y + _target.y,
			_coordinates->Rotate(0.0f, value * _deltaTime).ToCartesian().z + _target.z
		);
		break;
	default:
		break;
	}
}

/// <summary>
/// �J�����̈ʒu������������֐�
/// </summary>
/// <param name="azimth">���ʊp</param>
/// <param name="elevation">�p</param>
void
Dx12Wrapper::ResetCoordinates(float azimth, float elevation)
{
	InitVector();											//���_�A�����_�����Z�b�g

	_coordinates.reset(new SphericalCoordinates());			//���ʍ��W������SphericalCoordinates�C���X�^���X���쐬
	_coordinates->SetRadius(INIT_RAD);						//���a
	_coordinates->SetAzimth(azimth);						//���ʊp
	_coordinates->SetElevation(elevation);					//�p

	_eye = XMFLOAT3											//���_���W��ݒ�
	(
		_coordinates->ToCartesian().x + _target.x,
		_coordinates->ToCartesian().y + _target.y,
		_coordinates->ToCartesian().z + _target.z
	);
}

/// <summary>
/// �r���[�v���W�F�N�V�����p�r���[���Z�b�g����֐�
/// </summary>
void
Dx12Wrapper::SetScene()
{
	_mappedScene->view = XMMatrixLookAtLH(										//�s��E���W����������
		XMLoadFloat3(&_eye), XMLoadFloat3(&_target), XMLoadFloat3(&_up));

	if (_perspective)															//�������e
	{
		_mappedScene->proj = XMMatrixPerspectiveFovLH(XM_PIDIV4,
			static_cast<float>(_winSize.cx) / static_cast<float>(_winSize.cy),
			0.1f,
			1000.0f
		);
	}
	else																		//���s���e
	{
		_currentRad = _coordinates->GetRadius();

		_mappedScene->proj = XMMatrixOrthographicLH(
			static_cast<float>(_winSize.cx) / (XM_PI * (INIT_RAD / _currentRad)),
			static_cast<float>(_winSize.cy) / (XM_PI * (INIT_RAD / _currentRad)),
			0.1f, 1000.0f);
	}

	_mappedScene->light = _light;
	_mappedScene->eye = _eye;

	ID3D12DescriptorHeap* sceneHeaps[] = { _sceneDescHeap.Get() };				//�f�B�X�N���v�^�q�[�v���R�}���h���X�g�ɃZ�b�g
	_cmdList->SetDescriptorHeaps(1, sceneHeaps);

	_cmdList->SetGraphicsRootDescriptorTable(									//�q�[�v�̃n���h�������[�g�p�����[�^�Ɗ֘A�t��
		0, _sceneDescHeap->GetGPUDescriptorHandleForHeapStart());
}

/// <summary>
/// �Q�[����ʗp���\�[�X�̑J��(PRESENT��RENDER_TARGET)�ERTV�̃Z�b�g�����s����֐�
/// </summary>
void
Dx12Wrapper::BeginGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();									//���݂̃o�b�N�o�b�t�@�[�̃C���f�b�N�X���擾

	BarrierTransition(																		//�Q�[����ʗp���\�[�X��RENDER_TARGET�ɑJ��
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	auto rtvH = _rtvHeap->GetCPUDescriptorHandleForHeapStart();								//�C���f�b�N�X�ɉ�����RTV�q�[�v�̃n���h�����Y����
	rtvH.ptr += 
		bbIdx * _dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	auto dsvH = _dsvHeap->GetCPUDescriptorHandleForHeapStart();

	_cmdList->OMSetRenderTargets(1, &rtvH, true, &dsvH);									//RTV�EDSV�q�[�v���Z�b�g

	float clearColor[4] = { 0.2f,0.2f,0.2f,1.0f };											//�w�i�F��ݒ�

	_cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);							//RTV��������
	_cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);		//DSV��������
}

/// <summary>
/// �Q�[���p���\�[�X�̑J��(RENDER_TARGET��STATE_PRESENT)�����s����֐�
/// </summary>
void
Dx12Wrapper::EndGameDraw()
{
	auto bbIdx = _swapchain->GetCurrentBackBufferIndex();		//���݂̃o�b�N�o�b�t�@�[�̃C���f�b�N�X���擾

	BarrierTransition(											//�Q�[����ʗp���\�[�X��PRESENT�ɑJ��
		_backBuffers[bbIdx], 
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT);

	_cmdList->Close();											//�R�}���h���X�g�̃N���[�Y

	ID3D12CommandList* cmdlists[] = { _cmdList.Get() };			//�R�}���h���X�g�̎��s
	_cmdQueue->ExecuteCommandLists(1, cmdlists);

	WaitForCommandQueue();										//�����̓����҂����s��

	_cmdAllocator->Reset();										//�L���[�̃N���A
	_cmdList->Reset(_cmdAllocator.Get(), nullptr);				//�ēx�R�}���h���X�g�𒙂߂鏀��
}
/// <summary>
/// �����̓����҂����s���֐�
/// </summary>
void
Dx12Wrapper::WaitForCommandQueue()
{
	_cmdQueue->Signal(_fence.Get(), ++_fenceVal);					//�t�F���X�l���X�V

	if (_fence->GetCompletedValue() != _fenceVal)					//CPU��GPU�̃t�F���X�l���r���A��v����܂ŏ�����҂����킹��
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);	//�C�x���g�n���h�����擾�H

		_fence->SetEventOnCompletion(_fenceVal, event);				//�t�F���X�l��_fenceVal�ɂȂ������C�x���g��ʒm

		WaitForSingleObject(event, INFINITE);						//�C�x���g����������܂ő҂�������

		CloseHandle(event);											//�C�x���g�����
	}
}

/// <summary>
/// ���[�g�V�O�l�`���E�p�C�v���C���E�`����@���Z�b�g����֐�
/// </summary>
/// <param name="rootSignature">���[�g�V�O�l�`��</param>
/// <param name="pipeline">�p�C�v���C��</param>
/// <param name="topology">�`����@</param>
void
Dx12Wrapper::SetPipelines(ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipeline, D3D12_PRIMITIVE_TOPOLOGY topology)
{
	_cmdList->SetGraphicsRootSignature(rootSignature);	//���[�g�V�O�l�`�����Z�b�g
	_cmdList->SetPipelineState(pipeline);				//�p�C�v���C�����Z�b�g
	_cmdList->IASetPrimitiveTopology(topology);			//�`����@��ݒ�
}

/// <summary>
/// �t�F�[�h�C���^�A�E�g�f�[�^���V�F�[�_�[�ɔ��f����֐�
/// </summary>
void
Dx12Wrapper::UpdateFade()
{
	_mappedFactor->fade = clamp(_fade, 0.0f, 1.0f);		//���l��0�ȏ�1�ȉ��ɐ���
}

/// <summary>
/// �t�F�[�h�C���^�A�E�g�����s����֐�
/// </summary>
/// <param name="start">�t�F�[�h�l�̏����l</param>
/// <param name="end">�t�F�[�h�l�̍ŏI�l</param>
/// <param name="func">�t�F�[�h�������Ɏ��s����������</param>
void
Dx12Wrapper::Fade(float start, float end)
{
	_mtx.lock();

	float t = 0.0f;							//����
	_start = start;							//�t�F�[�h�l�̏����l
	_end = end;								//�t�F�[�h�l�̍ŏI�l

	_rate *= FADE_TIME;

	//���`��ԏ���
	for (int i = 0; i <= _rate; i++)
	{
		_fade = lerp(_start, _end, t);		//�t�F�[�h�l����`���

		t += _deltaTime / FADE_TIME;		//���Z

		Sleep(1);							//������҂�
	}

	_mtx.unlock();
}

/// <summary>
/// �o�b�t�@�[���쐬����֐�
/// </summary>
/// <param name="buff">�쐬�����o�b�t�@�[���i�[����|�C���^</param>
/// <param name="heapProp">�q�[�v�v���p�e�B</param>
/// <param name="resDesc">���\�[�X�f�B�X�N</param>
/// <param name="resState">���\�[�X�X�e�[�g</param>
HRESULT
Dx12Wrapper::CreateResource(
	ComPtr<ID3D12Resource>& buff,CD3DX12_HEAP_PROPERTIES heapProp,
	CD3DX12_RESOURCE_DESC resDesc,	D3D12_RESOURCE_STATES resState
)
{
	auto ret = _dev->CreateCommittedResource(	//�萔�o�b�t�@�[(���\�[�X)���쐬
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		resState,
		nullptr,
		IID_PPV_ARGS(&buff));

	return ret;
}

/// <summary>
/// �f�B�X�N���v�^�q�[�v���쐬����֐�
/// </summary>
/// <param name="heap">�q�[�v���i�[����|�C���^</param>
/// <param name="type">�q�[�v�^�C�v</param>
/// <param name="nodeMask">�m�[�h�}�X�N</param>
/// <param name="numDescriptor">�f�B�X�N���v�^��</param>
/// <param name="flag">�q�[�v�t���O</param>
/// <returns>�֐��������������ǂ���</returns>
HRESULT
Dx12Wrapper::CreateDescriptorHeap(
	ComPtr<ID3D12DescriptorHeap>& heap,	D3D12_DESCRIPTOR_HEAP_TYPE type,
	UINT nodeMask,UINT numDescriptor,D3D12_DESCRIPTOR_HEAP_FLAGS flag)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};							//�f�B�X�N���v�^�q�[�v�ݒ�\���̂̍쐬�E�ݒ�
	heapDesc.Type = type;
	heapDesc.NodeMask = nodeMask;
	heapDesc.NumDescriptors = numDescriptor;
	heapDesc.Flags = flag;

	auto result = _dev->CreateDescriptorHeap(							//�f�B�X�N���v�^�q�[�v�̍쐬
		&heapDesc, IID_PPV_ARGS(&heap));

	return result;
}

/// <summary>
/// �E��n�̃r���[�ϊ��s���Ԃ��֐�
/// </summary>
/// <returns>�s��</returns>
XMMATRIX
Dx12Wrapper::ViewMatrix()const
{
	XMFLOAT3 RHEye = XMFLOAT3(-_eye.x,_eye.y,_eye.z);
	XMFLOAT3 RHTarget = XMFLOAT3(-_target.x, _target.y, _target.z);

	return XMMatrixLookAtRH(XMLoadFloat3(&RHEye), XMLoadFloat3(&RHTarget), XMLoadFloat3(&_up));
}

/// <summary>
/// �E��n�̃v���W�F�N�V�����ϊ��s���Ԃ��֐�
/// </summary>
/// <returns>�s��</returns>
XMMATRIX
Dx12Wrapper::ProjMatrix()const
{
	if (_perspective)
	{
		return XMMatrixPerspectiveFovRH(
			XM_PIDIV4,ViewPort()->Width / ViewPort()->Height,
			0.1f,
			1000.0f
		);
	}
	else
	{
		return XMMatrixOrthographicRH(
			static_cast<float>(_winSize.cx) / (XM_PI * (INIT_RAD / _currentRad)),
			static_cast<float>(_winSize.cy) / (XM_PI * (INIT_RAD / _currentRad)),
			0.1f, 1000.0f);
	}
}

/// <summary>
/// EffectManager�C���X�^���X��Ԃ��֐�
/// </summary>
/// <returns>�C���X�^���X�̃|�C���^</returns>
EffectManager*
Dx12Wrapper::Effect()const
{
	return _effect.get();
}

/// <summary>
/// �f�o�C�X��Ԃ��֐�
/// </summary>
/// <returns>�f�o�C�X�̃|�C���^</returns>
ID3D12Device*
Dx12Wrapper::Device()const
{
	return _dev.Get();
}

/// <summary>
/// �X���b�v�`�F�[����Ԃ��֐�
/// </summary>
/// <returns>�X���b�v�`�F�[���̃|�C���^</returns>
IDXGISwapChain4*
Dx12Wrapper::Swapchain()const
{
	return _swapchain.Get();
}

/// <summary>
/// �R�}���h���X�g��Ԃ��֐�
/// </summary>
/// <returns>�R�}���h���X�g�̃|�C���^</returns>
ID3D12GraphicsCommandList*
Dx12Wrapper::CommandList()const
{
	return _cmdList.Get();
}

/// <summary>
/// �R�}���h�L���[��Ԃ��֐�
/// </summary>
/// <returns>�R�}���h�L���[�̃|�C���^</returns>
ID3D12CommandQueue*
Dx12Wrapper::CommandQueue()const
{
	return _cmdQueue.Get();
}

/// <summary>
/// �o�b�N�o�b�t�@�[�i1���ځj��Ԃ��֐�
/// </summary>
/// <returns></returns>
ID3D12Resource*
Dx12Wrapper::BackBuffer() const
{
	return _backBuffers[0];
}

/// <summary>
/// RTV�q�[�v��Ԃ��֐�
/// </summary>
/// <returns>RTV�q�[�v</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::RTVHeap() const
{
	return _rtvHeap.Get();
}

/// <summary>
/// �[�x�X�e���V���q�[�v��Ԃ��֐�
/// </summary>
/// <returns>�[�x�X�e���V���q�[�v</returns>
ID3D12DescriptorHeap*
Dx12Wrapper::DSVHeap() const
{
	return _dsvHeap.Get();
}

/// <summary>
/// �����p�q�[�v��Ԃ��֐�
/// </summary>
/// <returns>�����p�q�[�v</returns>
ID3D12DescriptorHeap* 
Dx12Wrapper::FactorCBVHeap() const
{
	return _factorCBVHeap.Get();
}

/// <summary>
/// �r���[�|�[�g��Ԃ��֐�
/// </summary>
/// <returns>�r���[�|�[�g</returns>
D3D12_VIEWPORT*
Dx12Wrapper::ViewPort() const
{
	return _viewPort.get();
}

/// <summary>
/// �V�U�[��`��Ԃ��֐�
/// </summary>
/// <returns>�V�U�[��`</returns>
D3D12_RECT*
Dx12Wrapper::Rect() const
{
	return _rect.get();
}

/// <summary>
/// ���_���W��Ԃ��֐�
/// </summary>
/// <returns>���_���W</returns>
XMVECTOR
Dx12Wrapper::Eye() const
{
	return XMLoadFloat3(&_eye);
}

/// <summary>
/// �������e���ǂ����Ԃ��֐�
/// </summary>
/// <returns>�^���l</returns>
bool
Dx12Wrapper::Perspective()const
{
	return _perspective;
}

/// <summary>
/// �J�����𓧎����e�ɂ��邩�ǂ������߂�֐�
/// </summary>
/// <param name="isPers">�������e���ǂ����itrue�Ȃ瓧���Afalse�Ȃ���s�j</param>
void
Dx12Wrapper::SetPersOrOrtho(bool isPers)
{
	_perspective = isPers;
}