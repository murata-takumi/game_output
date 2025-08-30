#pragma once
#include "Application.h"

class aiScene;
class BoneInfo;
class BoxCollider;
class ICollider;
class SphereCollider;
class Mesh;
class Vector3;
class FbxComposition
{
public:
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

	//�R���X�g���N�^�A�f�X�g���N�^
	FbxComposition() = default;
	~FbxComposition() = default;

	//���f��������������֐�
	void InitModel(const wchar_t* filePath);

	//���_�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateVertexBufferView();
	//�C���f�b�N�X�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateIndexBufferView();
	//�V�F�[�_�[���\�[�X�E�r���[�쐬�֐�
	HRESULT CreateShaderResourceView();

	//�����蔻��i��`�j�쐬�֐�
	void CreateBoxCollider(const Vector3& size, const Vector3& pos,IFbx* obj);

	//�����蔻��i���j�쐬�֐�
	void CreateSphereCollider(float radius, const Vector3& pos, IFbx* obj);

	//���t���[���̕`�揈��
	void Draw();

	//���t���[���̍��W�ϊ�����
	void Update();

	//�����蔻��̃|�C���^��Ԃ��֐�
	shared_ptr<ICollider>Collider()const;

	//�\���p���W���擾
	Vector3 CurrentPosition()const;

	//���ʍ��W���擾
	Vector3 FrontVec()const;

	//�������W���擾
	Vector3 FootVec()const;

	//���݂̑��x
	Vector3 Speed()const;

	//�I�u�W�F�N�g��
	const string Name();

	//�{�[���ϊ���r�����邩���߂�
	void SetRejectBone(bool val);

private:
	
};