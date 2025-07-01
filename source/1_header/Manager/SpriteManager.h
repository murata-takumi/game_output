#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/CommonStates.h>
#include <DirectXTK12-master/Inc/Effects.h>
#include <DirectXTK12-master/Inc/PrimitiveBatch.h>
#include <DirectXTK12-master/Inc/ResourceUploadBatch.h>
#include <DirectXTK12-master/Inc/SpriteBatch.h>
#include <DirectXTK12-master/Inc/SpriteFont.h>
#include <DirectXTK12-master/Inc/VertexTypes.h>

#include <sstream>
#include <time.h>

/// <summary>
/// �摜�╶���t�H���g���Ǘ�����N���X
/// </summary>
class BoxCollider;
class BoxCollider2D;
class Dx12Wrapper;
class IFBX;
class FBXActor;
class Vector3;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:	
	//�O���t�B�b�N�X������
	unique_ptr<GraphicsMemory> _gmemory;		
	//�t�H���g�\���p�I�u�W�F�N�g
	unique_ptr<SpriteFont> _spriteFont;
	//�X�v���C�g�i�摜�j�\���p�I�u�W�F�N�g
	unique_ptr<SpriteBatch> _spriteBatch;

	//�s�N�Z���V�F�[�_�[�p�f�[�^
	ID3DBlob* _psBlob = nullptr;											

	//�E�B���h�E�T�C�Y
	SIZE _winSize;															
	//��ʂ̕�
	LONG _width;															
	//��ʂ̍���
	LONG _height;															

	unique_ptr<BasicEffect> _effect;

	unique_ptr<PrimitiveBatch<VertexPositionColor>> _batch;

	//���[���h�s��
	SimpleMath::Matrix _world;												
	//�r���[�s��
	SimpleMath::Matrix _view;												
	//�v���W�F�N�V�����s��
	SimpleMath::Matrix _proj;												

	//�t�H���g�E�摜�p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _heapForSpriteFont = nullptr;				
	//�q�[�v�n���h��(CPU)
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								
	//�q�[�v�n���h��(GPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								
	//�n���h���̃A�h���X�̍���
	UINT _incrementSize;													

	//�n���h��(GPU)�̃x�N�g��
	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					

	unique_ptr<BoxCollider2D> _startColl;
	unique_ptr<BoxCollider2D> _exitColl;

	//���[�h���A�C�R���p��`
	RECT _loadingRect;
	//�w�i�p��`
	RECT _BGRect;														

	//�X�v���C�g�`��p���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> _spriteRS;									

	//X���̃O���b�h�̕�
	SimpleMath::Vector3 _XAxis = SimpleMath::Vector3(1000.f, 0.f, 0.0f);	
	//Y���̃O���b�h�̕�
	SimpleMath::Vector3 _YAxis = SimpleMath::Vector3(0.0f, 1000.f, 0.0f);	
	//Z���̃O���b�h�̕�
	SimpleMath::Vector3 _ZAxis = SimpleMath::Vector3(0.0f, 0.0f, 1000.f);	
	//���W�̒��S
	SimpleMath::Vector3 _origin = SimpleMath::Vector3::Zero;				

	//�R���X�g���N�^
	SpriteManager();
	SpriteManager(const SpriteManager&) = delete;
	//�f�X�g���N�^
	~SpriteManager();

	//SpriteBatch�p���[�g�V�O�l�`�����쐬����֐�
	HRESULT CreateSpriteRS();												
	//UI�p�̉摜�̃o�b�t�@�[�E�r���[���쐬����֐�
	HRESULT CreateUIBufferView(const wchar_t* path, string key);

	//�E����W��������W�ɕϊ�����֐�
	XMVECTOR RightPosToLeftPos(const Vector3& rightPos);

	//�X�v���C�g�E������\���p�I�u�W�F�N�g������������֐�
	void InitSpriteDevices();												

	//�����蔻���`�悷��֐�
	void ColliderDraw(const shared_ptr<BoxCollider> collider);

	//�}�E�X�J�[�\������`���ɂ��邩�m�F����
	bool IsCursorInRect(RECT rect);
public:
	static SpriteManager& Instance();

	void Init(LONG width, LONG height);

	//��ʃT�C�Y�̕ύX�����m���ċ�`�𒲐�����
	void AdjustSpriteRect();										

	//���[�h��ʂł̕`��
	void LoadingDraw();		

	//�^�C�g����ʂł̃{�^���̕`��
	void ButtonDraw();

	//�w�i�̕`��
	void BackGroundDraw();

	//�}�E�X�J�[�\����`��
	void GridDraw(const map<string, shared_ptr<IFBX>> actorAndObjs);

	//�O���t�B�b�N�X���������R�}���h���X�g�ɃZ�b�g
	void Commit();

	//�J�[�\�����X�^�[�g�{�^����ɂ��邩
	bool TitleIsOnStart();

	//�J�[�\�����I���{�^����ɂ��邩
	bool TitleIsOnExit();
};