#pragma once
#include "Application.h"

class aiAnimation;
class aiNode;
class aiScene;
class BoneData;
class BoneInfo;
class BoxCollider;
class ICollider;
class IFbx;
class SphereCollider;
class Mesh;
class Vector3;
class FbxComposition
{
public:
	//�R���X�g���N�^�A�f�X�g���N�^
	FbxComposition() = default;
	~FbxComposition() = default;

	//���f��������������֐�
	void InitModel(const wchar_t* filePath);

	//�����蔻��i��`�j�쐬�֐�
	void CreateBoxCollider(const Vector3& size, const Vector3& pos, IFbx* obj);

	//�����蔻��i���j�쐬�֐�
	void CreateSphereCollider(float radius, const Vector3& pos, IFbx* obj);

	//���t���[���̕`�揈��
	void Draw();

	//���t���[���̍��W�ϊ�����
	void Update();

	//�{�[���ϊ���r�����邩���߂�
	void SetRejectBone(bool val);

	//���_�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateVertexBufferView();
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateIndexBufferView();
	//�V�F�[�_�[���\�[�X�E�r���[�쐬�֐�
	HRESULT CreateShaderResourceView(); 
	//���W�ϊ��s��쐬�֐�
	HRESULT CreateTransformView(int buffLength);

	//���W�ϊ����s���s��
	XMMATRIX* MappedMats();

	//�ړ��p���W
	Vector3& TransrateVector();

	//���݂̑��x
	Vector3& Speed();

	//�{�[�����ƃC���f�b�N�X�̘A�z�z��
	map<string, unsigned int>& GetBoneMapping();

	//�{�[�����
	vector<BoneInfo>& GetBoneInfo();

	//�A�j���[�V������Ԃ��֐�
	aiAnimation** Animations()const;

	//���[�g�m�[�h��Ԃ�
	aiNode* RootNode()const;

	//�����蔻��̃|�C���^��Ԃ��֐�
	shared_ptr<ICollider>Collider()const;

	//�\���p���W���擾
	Vector3 CurrentPosition()const;

	//���ʍ��W���擾
	Vector3 FrontVec()const;

	//�������W���擾
	Vector3 FootVec()const;

	//�A�j���[�V���������擾
	int AnimationNumber()const;

	//�I�u�W�F�N�g��
	const string Name();

private:
	//���f���f�[�^��ێ�����|�C���^
	const aiScene* _scene;

	//�����蔻��̃|�C���^
	shared_ptr<ICollider> _collider;

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

	//�����蔻����I�u�W�F�N�g�̔����قǏ�ɂ��炷���߂̍s��
	XMMATRIX _shiftColMatrix;

	//�ړ��ʂ��i�[����x�N�g��
	Vector3 _translateVector;

	//ImGui�ł̕\���Ɏg�����W
	Vector3 _currentPosition;

	//���ʔ���Ɏg�p
	Vector3 _frontVec;

	//�W�����v��̃A�j���[�V�����J�ڂɎg�p
	Vector3 _footVec;

	//�I�u�W�F�N�g�̑��x
	Vector3 _speed;

	//�I�u�W�F�N�g�̖��O
	const string _name;

	//�����蔻��̍X�V����{�[���̍��W�ϊ���r�����邩
	bool _rejectBone;	

	//�s�񂪃[���s�񂩔��肷��
	bool IsZeroMatrix(const XMMATRIX& mat);
};