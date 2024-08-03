#pragma once
#include "Application.h"

#include <assimp/Importer.hpp>
#include <assimp/matrix4x4.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <d3d12.h>
#include <string>

struct aiMesh;
struct aiMaterial;

/// <summary>
/// FBX���f�����\�����郁�b�V���̍\����
/// </summary>
struct Mesh
{
	//���_�x�N�g��
	vector<FBXVertex> vertices;
	//�C���f�b�N�X�x�N�g��
	vector<uint32_t> indices;
	//�e�N�X�`���̃t�@�C���p�X
	wstring diffuseMap;
};

/// <summary>
/// �{�[�����̂��̂������
/// </summary>
struct BoneInfo
{
	//���_����]�̒��S�ֈړ�������s��
	XMMATRIX _boneOffset;
	//�ŏI�I�ȉ�]�s��
	XMMATRIX _finalTrans;
};

/// <summary>
/// ���f���f�[�^���󂯎��\����
/// </summary>
struct ImportSettings
{
	//�o�͐�̃��b�V���x�N�g��
	vector<Mesh>& meshes;
	//�{�[�����ƃC���f�b�N�X�̘A�z�z��
	map<string, unsigned int>& boneMapping;
	//�{�[�����
	vector<BoneInfo>& boneInfos;
	//U���W�𔽓]�����邩
	bool inverseU = false;
	//V���W�𔽓]�����邩
	bool inverseV = false;
};

class AssimpLoader
{
public:
	//�C���X�^���X�擾�֐�
	static AssimpLoader& Instance();

	//�V�[���f�[�^�����[�h����֐�
	const aiScene* LoadScene(const wchar_t* filePath);

	//���f�������[�h����֐�
	void LoadModel(const aiScene* scene, const wchar_t* filePath,		
		ImportSettings& import);

private:
	//�{�[����
	unsigned int _boneNum = 0;

	//�R���X�g���N�^
	AssimpLoader();
	AssimpLoader(const AssimpLoader&) = delete;
	//�f�X�g���N�^
	~AssimpLoader();

	//���b�V�������[�h����֐�
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	//���b�V�����̃{�[�������[�h����֐�
	void LoadBone(Mesh& dst, const aiMesh* src,										
		map<string, unsigned int>& boneMapping,vector<BoneInfo>& boneInfos);
	//�e�N�X�`�������[�h����֐�
	void LoadTexture(const wchar_t* filename,Mesh& dst,const aiMaterial* src);		

	//���_�Ƀ{�[��ID�ƃE�F�C�g��ݒ肷��֐�
	void SetIndexAndWeight(FBXVertex& vert,unsigned int id,float weight);			

	//���f���f�[�^�C���|�[�g�p�I�u�W�F�N�g
	Assimp::Importer _importer;														
};