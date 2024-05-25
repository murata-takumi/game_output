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
class Dx12Wrapper;
class FBXBase;
class FBXActor;
class SpriteManager
{
	template<typename T>
	using ComPtr = ComPtr<T>;

private:
	Dx12Wrapper& _dx12;														//Dx12Wrapper�C���X�^���X

	unique_ptr<GraphicsMemory> _gmemory;									//�O���t�B�b�N�X������
	unique_ptr<SpriteBatch> _spriteBatch;									//�X�v���C�g�i�摜�j�\���p�I�u�W�F�N�g

	vector<shared_ptr<FBXBase>> _actorAndObjs;								//FBXBase�y�єh���I�u�W�F�N�g�̃x�N�g��

	ID3DBlob* _psBlob = nullptr;											//�s�N�Z���V�F�[�_�[�p�f�[�^

	SIZE _winSize;															//�E�B���h�E�T�C�Y
	LONG _width;															//��ʂ̕�
	LONG _height;															//��ʂ̍���

	unique_ptr<BasicEffect> _effect;

	unique_ptr<PrimitiveBatch<VertexPositionColor>> _batch;

	SimpleMath::Matrix _world;												//���[���h�s��
	SimpleMath::Matrix _view;												//�r���[�s��
	SimpleMath::Matrix _proj;												//�v���W�F�N�V�����s��

	ComPtr<ID3D12DescriptorHeap> _heapForSpriteFont = nullptr;				//�t�H���g�E�摜�p�q�[�v
	D3D12_CPU_DESCRIPTOR_HANDLE _tmpCPUHandle;								//�q�[�v�n���h��(CPU)
	D3D12_GPU_DESCRIPTOR_HANDLE _tmpGPUHandle;								//�q�[�v�n���h��(GPU)
	UINT _incrementSize;													//�n���h���̃A�h���X�̍���

	map<string,D3D12_GPU_DESCRIPTOR_HANDLE> _GPUHandles;					//�n���h��(GPU)�̃x�N�g��

	RECT _loadingRect;														//���[�h���A�C�R���p��`

	ComPtr<ID3D12RootSignature> _spriteRS;									//�X�v���C�g�`��p���[�g�V�O�l�`��

	SimpleMath::Vector3 _XAxis = SimpleMath::Vector3(1000.f, 0.f, 0.0f);	//X���̃O���b�h�̕�
	SimpleMath::Vector3 _YAxis = SimpleMath::Vector3(0.0f, 1000.f, 0.0f);	//Y���̃O���b�h�̕�
	SimpleMath::Vector3 _ZAxis = SimpleMath::Vector3(0.0f, 0.0f, 1000.f);	//Z���̃O���b�h�̕�
	SimpleMath::Vector3 _origin = SimpleMath::Vector3::Zero;				//

	HRESULT CreateSpriteRS();												//SpriteBatch�p���[�g�V�O�l�`�����쐬����֐�
	HRESULT CreateUIBufferView(const wchar_t* path, string key);			//UI�p�̉摜�̃o�b�t�@�[�E�r���[���쐬����֐�

	void InitSpriteDevices();												//�X�v���C�g�E������\���p�I�u�W�F�N�g������������֐�

	void ColliderDraw(const shared_ptr<BoxCollider> collider);
public:
	SpriteManager(Dx12Wrapper& dx12,LONG width,LONG height);		//�R���X�g���N�^

	void AdjustSpriteRect();										//��ʃT�C�Y�̕ύX�����m���ċ�`�𒲐�����

	void LoadingDraw();												//���[�h��ʂł̕`��
	void GridDraw();												//�}�E�X�J�[�\����`��

	void Commit();													//�O���t�B�b�N�X���������R�}���h���X�g�ɃZ�b�g

	void SetActors(vector<shared_ptr<FBXBase>> actorAndObjects);	//�A�N�^�[��ݒ肷��֐�
};