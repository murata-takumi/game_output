#pragma once
#include "Application.h"
#include "FBX/AssimpLoader.h"
#include "FBX/BoxCollider.h"

/// <summary>
/// FBX���f���̔h�����ƂȂ�N���X
/// </summary>
class Dx12Wrapper;
class AssimpLoader;
class ImGuiManager;
class FBXBase
{
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;

protected:
	//�֐��̕Ԃ�l
	HRESULT result;							

	//���f���f�[�^��ێ�����|�C���^
	const aiScene* _scene;													

	//�����蔻��̃|�C���^
	shared_ptr<BoxCollider> _collider;										

	//���f���ǂݍ��ݗp���b�V���z��
	vector<Mesh> _meshes;												
	//�{�[�����ƃC���f�b�N�X�̘A�z�z��
	map<string, unsigned int> _boneMapping;							
	//�{�[�����̃x�N�g��
	vector<BoneInfo> _boneInfo;												

	//���b�V���p���_�o�b�t�@�[�r���[�z��
	vector<D3D12_VERTEX_BUFFER_VIEW> _vbViews;

	//���b�V���p�C���f�b�N�X�o�b�t�@�[�r���[�z��
	vector<D3D12_INDEX_BUFFER_VIEW> _ibViews;								

	//���[���h�s��p�o�b�t�@�[
	ComPtr<ID3D12Resource> _transBuffer;								
	//���[���h�s��p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _transHeap;								

	//�e�N�X�`���p�q�[�v
	ComPtr<ID3D12DescriptorHeap> _texHeap;
	//�e�N�X�`���o�b�t�@�[�r���[��GPU�n���h���z��
	vector<D3D12_GPU_DESCRIPTOR_HANDLE> _gpuHandles;						

	//���f���̍��W�ϊ��s����i�[����\���̂̃|�C���^
	XMMATRIX* _mappedMats = nullptr;

	XMMATRIX _motionMat;

	vector<FBXVertex> _normals;

	//���W
	XMVECTOR _pos;

	//��������Ɏg�p
	XMVECTOR _footVec;

	//�����蔻��̍X�V����{�[���̍��W�ϊ���r�����邩
	bool _rejectBone;

	//���f��������������֐�
	void InitModel(const wchar_t* filePath);								

	//���_�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateVertexBufferView();					
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateIndexBufferView();						
	//�V�F�[�_�[���\�[�X�E�r���[�쐬�֐�
	HRESULT CreateShaderResourceView();										
	//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬�֐��i�p����Ŏg�p�j
	virtual HRESULT CreateTransformView() = 0;								

public:
	//�R���X�g���N�^
	FBXBase(const wchar_t* filePath,const XMFLOAT3& size,const XMFLOAT3& pos);
	//�f�X�g���N�^
	virtual ~FBXBase();													

	//���t���[���̕`�揈��
	void Draw();	

	//���t���[���̍��W�ϊ�����
	virtual void Update();											

	//�����蔻��̃|�C���^��Ԃ��֐�
	shared_ptr<BoxCollider>Collider()const;		

	XMVECTOR& Pos();

	XMVECTOR FootVec()const;

	//�{�[���ϊ���r�����邩���߂�
	void SetRejectBone(bool val);
};