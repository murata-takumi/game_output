#include "Application.h"

/// <summary>
/// �J�������ǂ���̕��ɉ�]�����邩���肷��񋓑�
/// </summary>
enum Degree
{
	Azimth,			//���ʊp
	Elevation,		//�p
};

/// <summary>
/// �J�������ǂ���̕����֕��s�ړ������邩���肷��񋓑�
/// </summary>
enum Direction
{
	Horizontal,	//��������
	Vertical,	//��������
	Depth,		//���s����
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
	SIZE _winSize;												//�E�B���h�E�T�C�Y

	shared_ptr<EffectManager> _effect;							//�G�t�F�N�g�Ǘ��p�C���X�^���X
	shared_ptr<SpriteManager> _sprite;							//�X�v���C�g�Ǘ��C���X�^���X
	shared_ptr<PeraRenderer> _pera;								//UI�Ǘ��C���X�^���X
	shared_ptr<ImGuiManager> _imgui;							//�f�o�b�O�p�E�B���h�E�Ǘ��C���X�^���X
	shared_ptr<SphericalCoordinates> _coordinates;				//�J�������W�p�C���X�^���X

	//DXGI����
	ComPtr<IDXGIFactory6> _dxgiFactory;							//�t�@�N�g���[
	ComPtr<ID3D12Device> _dev;									//�f�o�C�X

	//DX12(�R�}���h)����
	ComPtr<ID3D12CommandAllocator> _cmdAllocator;				//�R�}���h�A���P�[�^
	ComPtr<ID3D12GraphicsCommandList> _cmdList;					//�R�}���h���X�g
	ComPtr<ID3D12CommandQueue> _cmdQueue;						//�R�}���h�L���[
	ComPtr<IDXGISwapChain4> _swapchain;							//�X���b�v�`�F�[��

	//�\���֘A�̃o�b�t�@���
	vector<ID3D12Resource*> _backBuffers;						//�����_�[�^�[�Q�b�g�p�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _rtvHeap;						//�����_�[�^�[�Q�b�g�r���[�p�q�[�v
	ComPtr<ID3D12Resource> _depthBuffer;						//�[�x�X�e���V���p�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _dsvHeap;						//�[�x�X�e���V���r���[�p�q�[�v

	unique_ptr<D3D12_VIEWPORT> _viewPort = {};					//�r���[�|�[�g
	unique_ptr<D3D12_RECT> _rect = {};							//�V�U�[��`

	//�r���[�v���W�F�N�V�����p�萔�o�b�t�@����
	ComPtr<ID3D12Resource> _sceneConstBuff;						//�r���[�E�v���W�F�N�V�����p�o�b�t�@�[
	ComPtr<ID3D12DescriptorHeap> _sceneDescHeap;				//�r���[�E�v���W�F�N�V�����p�f�B�X�N���v�^�q�[�v

	ComPtr<ID3D12DescriptorHeap> _factorCBVHeap;				//�c�݃f�[�^�p�q�[�v
	ComPtr<ID3D12Resource> _factorConstBuff;					//�c�݃f�[�^�p�o�b�t�@�[

	ComPtr<ID3D12DescriptorHeap> _heapForImgui;					//imgui�Ŏg�p����q�[�v

	/// <summary>
	/// �V�F�[�_�[�Ƀt�F�[�h�C���^�A�E�g�f�[�^��n�����߂̍\���̋y�уC���X�^���X
	/// </summary>
	struct Factor
	{
		float fade;												//�t�F�[�h�C���^�A�E�g�f�[�^
	};
	Factor* _mappedFactor = nullptr;							//�}�b�v�p�|�C���^

	/// <summary>
	/// �X���b�g�ɗ������ރf�[�^�̍\���̋y�уC���X�^���X
	/// �e�s���ʌɗ������ނ̂ł͂Ȃ��A�\���̂Ƃ��Ă܂Ƃ߂�
	/// </summary>
	struct SceneData
	{
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT3 eye;
	};
	SceneData* _mappedScene;									//�}�b�v�p�|�C���^

	ComPtr<ID3D12Fence> _fence;									//�t�F���X
	UINT64 _fenceVal;											//�������l

	CD3DX12_RESOURCE_BARRIER _barrier;							//���\�[�X�o���A�p�ϐ�

	XMFLOAT3 _eye;												//���_���W
	XMFLOAT3 _target;											//�����_���W
	XMFLOAT3 _up;												//����W

	XMFLOAT3 _initEye;											//���_�̏������W
	XMFLOAT3 _initTarget;										//�����_�̏������W

	mutex _mtx;													//�r������p�I�u�W�F�N�g

	bool _perspective;											//�������e���ǂ������߂�^���l

	float _initRad;												//���a�̏����l
	float _currentRad;
	float _deltaTime;											//�t���[���Ԃ̎���
	float _fade;												//�t�F�[�h�C���^�A�E�g���邩�ǂ��������߂�f�[�^
	float _start, _end;											//�t�F�[�h�C���^�A�E�g�Ő��`�⊮����ۂ̏����l�A�ŏI�l

	int _rate;													//�t���[�����[�g
		
	using LoadLambda_t =										//�e�N�X�`�����[�h�p�����_�����i�[����^�̒�`
		function<HRESULT(
			const wstring& path, TexMetadata*, ScratchImage&
		)>;

	map<string, ID3D12Resource*> _resourceTable;				//string��key,���\�[�X��value�Ƃ����A�z�z��

	HRESULT InitializeDXGIDevice();								//�f�o�C�X�֘A������������֐�

	HRESULT InitializeCommand();								//�R�}���h�֘A������������֐�

	HRESULT CreateSwapChain(const HWND& hwnd);					//�X���b�v�`�F�[�����쐬����֐�

	HRESULT CreateRenderTargetsView();							//�����_�[�^�[�Q�b�g���쐬����֐�

	HRESULT CreateSceneView();									//�r���[�v���W�F�N�V�����p�r���[���쐬����֐�

	HRESULT CreateDepthStencilView();							//�[�x�X�e���V���r���[���쐬����֐�

	HRESULT CreateFactorBufferAndView();						//�G�t�F�N�g�K�p�����߂�f�[�^�p�̃q�[�v�E�o�b�t�@�[���쐬����֐�

	void CreateTextureLoaderTable();							//�e�N�X�`�����[�g�p�e�[�u�����쐬����֐�

	void InitVector();											//���_�A�����_�̃x�N�g��������������֐�

public:
	map<string, LoadLambda_t> _loadLambdaTable;							//string��key,LoadLambda_t��value�Ƃ����A�z�z��

	Dx12Wrapper(HWND hwnd, float deltatime);							//�R���X�g���N�^

	void BarrierTransition(												//���\�[�X��J�ڂ�����֐�
		ID3D12Resource* resource,
		D3D12_RESOURCE_STATES before, 
		D3D12_RESOURCE_STATES after);

	XMVECTOR GetXZVecEyeToTarget()const;								//���_���W�i�J�������W�j���璍���_���W�ւ̃x�N�g�����擾����֐�
	XMVECTOR GetRightVector()const;										//�����_�ւ̃x�N�g���̉E�����̃x�N�g�����擾����֐�

	float GetDeltaTime()const;

	void ScalingCoordinates(int x);										//�J�������ߕt����E��������֐�
	void TranslateCoordinates(XMVECTOR vec);							//�J�������ړ�������֐�
	void RotateCoordinates(Degree deg, float value);					//�J��������]������֐�
	void ResetCoordinates(float azimth,float elevation);				//�J�����̈ʒu������������֐�

	void SetScene();													//�r���[�v���W�F�N�V�����p�r���[���R�}���h���X�g�ɃZ�b�g����֐�

	void BeginGameDraw();												//���\�[�X�̑J��(STATE_PRESENT��RENDER_TARGET)�ERTV�̃Z�b�g�����s����֐�
	void EndGameDraw();													//���\�[�X�̑J��(RENDER_TARGET��STATE_PRESENT)�����s����֐�

	void WaitForCommandQueue();											//�����̓����҂����s���֐�

	void SetPipelines(ID3D12RootSignature* rootSignature,				//���[�g�V�O�l�`���E�p�C�v���C���E�`����@���Z�b�g����֐�
		ID3D12PipelineState* pipeline, 
		D3D12_PRIMITIVE_TOPOLOGY topology);

	void UpdateFade();													//�t�F�[�h�C���^�A�E�g�f�[�^���V�F�[�_�[�ɔ��f����֐�

	void Fade(float start,float end);									//�t�F�[�h�C���^�A�E�g�����s����֐�

	HRESULT CreateResource(												//�o�b�t�@�[���쐬����֐�
		ComPtr<ID3D12Resource>& buff,
		CD3DX12_HEAP_PROPERTIES heapProp,
		CD3DX12_RESOURCE_DESC resDesc,
		D3D12_RESOURCE_STATES resState
	);

	HRESULT CreateDescriptorHeap(										//�f�B�X�N���v�^�q�[�v���쐬����֐�
		ComPtr<ID3D12DescriptorHeap>& heap,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		UINT nodeMask,
		UINT numDescriptor,
		D3D12_DESCRIPTOR_HEAP_FLAGS flag
	);

	XMMATRIX ViewMatrix()const;											//�r���[�s���Ԃ��֐�
	XMMATRIX ProjMatrix()const;											//�v���W�F�N�V�����s���Ԃ��֐�

	EffectManager* Effect() const;										//EffectManager�C���X�^���X��Ԃ��֐�
	SpriteManager* Sprite() const;										//SpriteManager�C���X�^���X��Ԃ��֐�
	PeraRenderer* Pera() const;											//PeraRenderer�C���X�^���X��Ԃ��֐�
	ImGuiManager* ImGui() const;										//ImGuiManager�C���X�^���X��Ԃ��֐�

	ID3D12Device* Device() const;										//�f�o�C�X��Ԃ��֐�
	IDXGISwapChain4* Swapchain() const;									//�X���b�v�`�F�[����Ԃ��֐�
	ID3D12GraphicsCommandList* CommandList() const;						//�R�}���h���X�g��Ԃ��֐�
	ID3D12CommandQueue* CommandQueue() const;							//�R�}���h�L���[��Ԃ��֐�
	ID3D12Resource* BackBuffer() const;									//�o�b�N�o�b�t�@�[�i1���ځj��Ԃ��֐�
	ID3D12DescriptorHeap* RTVHeap() const;								//RTV�q�[�v��Ԃ��֐�
	ID3D12DescriptorHeap* DSVHeap() const;								//�[�x�X�e���V���q�[�v��Ԃ��֐�
	ID3D12DescriptorHeap* FactorCBVHeap() const;						//�����p�q�[�v��Ԃ��֐�
	D3D12_VIEWPORT* ViewPort() const;									//�r���[�|�[�g��Ԃ��֐�
	D3D12_RECT* Rect() const;											//�V�U�[��`��Ԃ��֐�

	bool Perspective();													//�������e���ǂ����Ԃ��֐�

	void SetPersOrOrtho(bool isPers);									//���s���e���������e�����߂�֐�
};

