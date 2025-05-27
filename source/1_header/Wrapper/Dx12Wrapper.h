#pragma once
#include "Includes.h"
#include "Functions.h"
#include "Vector3.h"

/// <summary>
/// �J�������ǂ���̕��ɉ�]�����邩���肷��񋓑�
/// </summary>
enum Degree
{
	//���ʊp
	Azimth,
	//�p
	Elevation,		
};

/// <summary>
/// �J�������ǂ���̕����֕��s�ړ������邩���肷��񋓑�
/// </summary>
enum Direction
{
	//��������
	Horizontal,	
	//��������
	Vertical,	
	//���s����
	Depth,		
};

/// <summary>
/// �`�施�߂����s���邽�߂̃I�u�W�F�N�g�����Ǘ�����N���X
/// </summary>
class EffectManager;
class SphericalCoordinates;
class SpriteManager;
class PeraRenderer;
class Renderer;
class ImGuiManager;
class Dx12Wrapper
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	//�E�B���h�E�T�C�Y
	SIZE _winSize;												

	//�G�t�F�N�g�Ǘ��p�C���X�^���X
	shared_ptr<EffectManager> _effect;					
	//UI�Ǘ��C���X�^���X
	shared_ptr<PeraRenderer> _pera;						
	//�J�������W�p�C���X�^���X
	shared_ptr<SphericalCoordinates> _coordinates;				

	//DXGI����
	//�t�@�N�g���[
	ComPtr<IDXGIFactory6> _dxgiFactory;							
	//�f�o�C�X
	ComPtr<ID3D12Device> _dev;									

	//DX12(�R�}���h)����
	//�R�}���h�A���P�[�^
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;				
	//�R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> _cmdList;					
	//�R�}���h�L���[
	ComPtr<ID3D12CommandQueue> _cmdQueue;						
	//�X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> _swapchain;							

	//�\���֘A�̃o�b�t�@���
	//�����_�[�^�[�Q�b�g�p�o�b�t�@�[
	vector<ID3D12Resource*> _backBuffers;						
	//�����_�[�^�[�Q�b�g�r���[�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;						
	//�[�x�X�e���V���p�o�b�t�@�[
	ComPtr<ID3D12Resource> _depthBuffer;						
	//�[�x�X�e���V���r���[�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;						

	//�r���[�|�[�g
	unique_ptr<D3D12_VIEWPORT> _viewPort = {};					
	//�V�U�[��`
	unique_ptr<D3D12_RECT> _rect = {};							

	//�r���[�v���W�F�N�V�����p�萔�o�b�t�@����
	//�r���[�E�v���W�F�N�V�����p�o�b�t�@�[
	ComPtr<ID3D12Resource> _sceneConstBuff;						
	//�r���[�E�v���W�F�N�V�����p�f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap;				

	//�c�݃f�[�^�p�o�b�t�@�[
	ComPtr<ID3D12Resource> _factorConstBuff;
	//�c�݃f�[�^�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _factorCBVHeap;					

	//imgui�Ŏg�p����q�[�v
	ComPtr<ID3D12DescriptorHeap> _heapForImgui;					

	/// <summary>
	/// �V�F�[�_�[�Ƀt�F�[�h�C���^�A�E�g�f�[�^��n�����߂̍\���̋y�уC���X�^���X
	/// </summary>
	struct Factor
	{
		float fade;
	};
	//�}�b�v�p�|�C���^
	Factor* _mappedFactor = nullptr;							

	/// <summary>
	/// �X���b�g�ɗ������ރf�[�^�̍\���̋y�уC���X�^���X
	/// �e�s���ʌɗ������ނ̂ł͂Ȃ��A�\���̂Ƃ��Ă܂Ƃ߂�
	/// </summary>
	struct SceneData
	{
		XMMATRIX view;
		XMMATRIX proj;
		Vector3 light;
		Vector3 eye;
	};
	//�}�b�v�p�|�C���^
	SceneData* _mappedScene;									

	//�t�F���X
	ComPtr<ID3D12Fence> _fence;									
	//�������l
	UINT64 _fenceVal;											

	//���\�[�X�o���A�p�ϐ�
	CD3DX12_RESOURCE_BARRIER _barrier;							

	//���_�i�J�����j���W
	Vector3 _eye;
	//�����_���W
	Vector3 _target;
	//����W
	Vector3 _up;
	//���C�g���W
	Vector3 _light;

	//���_�̏������W
	Vector3 _initEye;											
	//�����_�̏������W
	Vector3 _initTarget;

	//�r������p�I�u�W�F�N�g
	mutex _mtx;													

	//�������e���ǂ������߂�^���l
	bool _perspective;

	//���݂̔��a
	float _currentRad;
	//�t���[���Ԃ̎���
	float _deltaTime;											
	//�t�F�[�h�C���E�A�E�g�ɗp����ϐ�
	float _fade;												
	//�t�F�[�h�C���E�A�E�g�Ő��`�⊮����ۂ̏����l�A�ŏI�l
	float _start, _end;											

	//�t���[�����[�g
	int _rate;													
		
	//�e�N�X�`�����[�h�p�����_�����i�[����^�̒�`
	using LoadLambda_t =										
		function<HRESULT(
			const wstring& path, TexMetadata*, ScratchImage&
		)>;

	//string��key,���\�[�X��value�Ƃ����A�z�z��
	map<string, ID3D12Resource*> _resourceTable;

	//�R���X�g���N�^
	Dx12Wrapper();
	Dx12Wrapper(const Dx12Wrapper&) = delete;
	//�f�X�g���N�^
	~Dx12Wrapper();

	//�f�o�C�X�֘A������������֐�
	HRESULT InitializeDXGIDevice();								

	//�R�}���h�֘A������������֐�
	HRESULT InitializeCommand();								

	//�X���b�v�`�F�[�����쐬����֐�
	HRESULT CreateSwapChain(const HWND& hwnd);					

	//�����_�[�^�[�Q�b�g���쐬����֐�
	HRESULT CreateRenderTargetsView();							

	//�r���[�v���W�F�N�V�����p�r���[���쐬����֐�
	HRESULT CreateSceneView();									

	//�[�x�X�e���V���r���[���쐬����֐�
	HRESULT CreateDepthStencilView();							

	//�G�t�F�N�g�K�p�����߂�f�[�^�p�̃q�[�v�E�o�b�t�@�[���쐬����֐�
	HRESULT CreateFactorBufferAndView();						

	//�e�N�X�`�����[�g�p�e�[�u�����쐬����֐�
	void CreateTextureLoaderTable();							

	//���_�A�����_�̃x�N�g��������������֐�
	void InitVector();											

public:
	//string��key,LoadLambda_t��value�Ƃ����A�z�z��
	map<string, LoadLambda_t> _loadLambdaTable;							

	//�V���O���g����Ԃ��֐�
	static Dx12Wrapper& Instance();

	//�������֐�
	void Init(HWND hwnd, float deltatime);

	//���\�[�X��J�ڂ�����֐�
	void BarrierTransition(												
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES before, 
		D3D12_RESOURCE_STATES after);

	//���_���W�i�J�������W�j���璍���_���W�ւ̃x�N�g�����擾����֐�
	Vector3 GetXZVecEyeToTarget()const;
	//�����_�ւ̃x�N�g���̉E�����̃x�N�g�����擾����֐�
	Vector3 GetRightVector()const;

	//�O�t���[���ƌ��t���[���̎��Ԃ̍������擾����֐�
	float GetDeltaTime()const;

	//�J�������ߕt����E��������֐�
	void ScalingCoordinates(int x);								
	//�J�������ړ�������֐�
	void SetCoordinatesCenter(Vector3 vec);							
	//�J��������]������֐�
	void RotateCoordinates(Degree deg, float value);					
	//�J�����̈ʒu������������֐�
	void ResetCoordinates(float azimth,float elevation);				

	//�r���[�v���W�F�N�V�����p�r���[���R�}���h���X�g�ɃZ�b�g����֐�
	void SetScene();													

	//���\�[�X�̑J��(STATE_PRESENT��RENDER_TARGET)�ERTV�̃Z�b�g�����s����֐�
	void BeginGameDraw();												
	//���\�[�X�̑J��(RENDER_TARGET��STATE_PRESENT)�����s����֐�
	void EndGameDraw();													

	//�����̓����҂����s���֐�
	void WaitForCommandQueue();											

	//���[�g�V�O�l�`���E�p�C�v���C���E�`����@���Z�b�g����֐�
	void SetPipelines(ID3D12RootSignature* rootSignature,				
		ID3D12PipelineState* pipeline, 
		D3D12_PRIMITIVE_TOPOLOGY topology);

	//�t�F�[�h�C���E�A�E�g�f�[�^���V�F�[�_�[�ɔ��f����֐�
	void UpdateFade();													
	//�t�F�[�h�C���E�A�E�g�����s����֐�
	void Fade(float start,float end);									

	//�o�b�t�@�[���쐬����֐�
	HRESULT CreateResource(												
		ComPtr<ID3D12Resource>& buff,
		CD3DX12_HEAP_PROPERTIES heapProp,
		CD3DX12_RESOURCE_DESC resDesc,
		D3D12_RESOURCE_STATES resState
	);
	//�f�B�X�N���v�^�q�[�v���쐬����֐�
	HRESULT CreateDescriptorHeap(										
		ComPtr<ID3D12DescriptorHeap>& heap,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		UINT nodeMask,
		UINT numDescriptor,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag
	);

	//�r���[�s���Ԃ��֐�
	XMMATRIX ViewMatrix()const;					
	//�v���W�F�N�V�����s���Ԃ��֐�
	XMMATRIX ProjMatrix()const;											

	//EffectManager�C���X�^���X��Ԃ��֐�
	EffectManager* Effect() const;									

	//�f�o�C�X��Ԃ��֐�
	ID3D12Device* Device() const;										
	//�X���b�v�`�F�[����Ԃ��֐�
	IDXGISwapChain4* Swapchain() const;		
	//�R�}���h���X�g��Ԃ��֐�
	ID3D12GraphicsCommandList* CommandList() const;				
	//�R�}���h�L���[��Ԃ��֐�
	ID3D12CommandQueue* CommandQueue() const;							
	//�o�b�N�o�b�t�@�[�i1���ځj��Ԃ��֐�
	ID3D12Resource* BackBuffer() const;									
	//RTV�q�[�v��Ԃ��֐�
	ID3D12DescriptorHeap* RTVHeap() const;								
	//�[�x�X�e���V���q�[�v��Ԃ��֐�
	ID3D12DescriptorHeap* DSVHeap() const;								
	//�����p�q�[�v��Ԃ��֐�
	ID3D12DescriptorHeap* FactorCBVHeap() const;						
	//�r���[�|�[�g��Ԃ��֐�
	D3D12_VIEWPORT* ViewPort() const;									
	//�V�U�[��`��Ԃ��֐�
	D3D12_RECT* Rect() const;

	//���_���W��Ԃ��֐�
	Vector3 Eye() const;

	//�������e���ǂ����Ԃ��֐�
	bool Perspective()const;
	//���s���e���������e�����߂�֐�
	void SetPersOrOrtho(bool isPers);									
};

