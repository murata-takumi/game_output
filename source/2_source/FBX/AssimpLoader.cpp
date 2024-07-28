#include "Functions.h"
#include "FBX/AssimpLoader.h"

#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// aiMatrix4x4���󂯎��XMMATRIX��Ԃ��֐�
/// </summary>
/// <param name="mat">aiMatrix4x4</param>
/// <returns>XMMATRIX</returns>
XMMATRIX LoadMatrix4x4(const aiMatrix4x4& mat)
{
	XMFLOAT4X4 ret = XMFLOAT4X4
	(
		mat.a1, mat.b1, mat.c1, mat.d1,
		mat.a2, mat.b2, mat.c2, mat.d2,
		mat.a3, mat.b3, mat.c3, mat.d3,
		mat.a4, mat.b4, mat.c4, mat.d4
	);

	return XMLoadFloat4x4(&ret);
}

/// <summary>
/// �R���X�g���N�^
/// ���ɏ����͂��Ȃ�
/// </summary>
AssimpLoader::AssimpLoader()
{

}

/// <summary>
/// �f�X�g���N�^
/// ���ɏ����͂��Ȃ�
/// </summary>
AssimpLoader::~AssimpLoader()
{

}

/// <summary>
/// �ÓI��AssimpLoader�C���X�^���X���擾
/// </summary>
/// <returns>�ÓI�C���X�^���X</returns>
AssimpLoader&
AssimpLoader::Instance()
{
	static AssimpLoader instance;
	return instance;
}

/// <summary>
/// ���f�����������Ă���C���X�^���X���擾����֐�
/// </summary>
/// <param name="filePath">���f���̃p�X</param>
/// <returns>���f�����������Ă���C���X�^���X</returns>
const aiScene*
AssimpLoader::LoadScene(const wchar_t* filePath)
{
	//�t�@�C�������������������珈�����f
	if (filePath == nullptr)
	{
		assert(0);
		return nullptr;
	}

	//�p�X��string�^�ɕς���
	auto path = ToString(filePath);
	
	//�r�b�g���Z���s���A�C���|�[�g���̐ݒ�t���O��ݒ肷��
	int flag = 0;
	flag |= aiProcess_Triangulate;
	flag |= aiProcess_CalcTangentSpace;
	flag |= aiProcess_GenSmoothNormals;
	flag |= aiProcess_GenUVCoords;
	flag |= aiProcess_RemoveRedundantMaterials;
	flag |= aiProcess_OptimizeMeshes;
	flag |= aiProcess_JoinIdenticalVertices;

	//�C���|�[�^�[�Ƀt�@�C���p�X�ƃt���O��ǂݍ��܂���
	auto ret = _importer.ReadFile(path, flag);
	if (ret == nullptr)
	{
		assert(0);
		return nullptr;
	}

	return ret;
}

/// <summary>
/// ���f���p�̏����������݁A�Ԃ��֐�
/// </summary>
/// <param name="scene">���f���������C���X�^���X</param>
/// <param name="filePath">���f���̃p�X</param>
/// <param name="import">�������ݐ�̍\����</param>
void
AssimpLoader::LoadModel(const aiScene* scene, const wchar_t* filePath, ImportSettings& import)
{
	//���b�V���z��
	auto& meshes = import.meshes;
	//�{�[���}�b�v
	auto& mapping = import.boneMapping;
	//�{�[�����
	auto& info = import.boneInfos;
	//U���W�𔽓]�����邩
	auto inverseU = import.inverseU;
	//V���W�𔽓]�����邩
	auto inverseV = import.inverseV;

	//���b�V���������Ă���ꍇ�ȉ��̏������s��
	if (0 < scene->mNumMeshes)
	{
		//���b�V�������Z�b�g
		meshes.clear();
		meshes.resize(scene->mNumMeshes);
		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			//�e���b�V�����擾
			const aiMesh* pMesh = scene->mMeshes[i];

			//���b�V���E�{�[���̓ǂݍ���
			LoadMesh(meshes[i], pMesh, inverseU, inverseV);									
			LoadBone(meshes[i], pMesh,mapping,info);

			//�e�}�e���A�����擾
			const aiMaterial* pMaterial = scene->mMaterials[pMesh->mMaterialIndex];
			//���b�V���Ƀ}�e���A����ǂݍ��܂���
			LoadTexture(filePath, meshes[i], pMaterial);
		}
	}
}

/// <summary>
/// FBX�̃��b�V����ǂݍ��ފ֐�
/// </summary>
/// <param name="dst">���b�V���z��</param>
/// <param name="src">FBX���f�����̃��b�V��</param>
/// <param name="inverseU">U���W�𔽓]�����邩</param>
/// <param name="inverseV">V���W�𔽓]�����邩</param>
void
AssimpLoader::LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV)
{
	//���b�V�����\�����钸�_���W�̏����l
	aiVector3D zero3D(0.0f, 0.0f, 0.0f);
	//���_�J���[�̏����l
	aiColor4D zeroColor(0.0f, 0.0f, 0.0f, 0.0f);

	//���_�����Đݒ�
	dst.vertices.resize(src->mNumVertices);

	//���_���ɏ�������
	for (auto i = 0u; i < src->mNumVertices; ++i)
	{
		//���_���W
		auto position = (src->mVertices[i]);
		//�@��
		auto normal = (src->mNormals[i]);
		//UV���W
		auto uv = (src->HasTextureCoords(0)) ? &(src->mTextureCoords[0][i]) : &zero3D;
		//����
		auto tangent = (src->HasTangentsAndBitangents()) ? &(src->mTangents[i]) : &zero3D;
		//���_�J���[
		auto color = (src->HasVertexColors(0)) ? &(src->mColors[0][i]) : &zeroColor;

		//���]�I�v�V��������������UV�𔽓]������
		if (inverseU)
		{
			uv->x = 1 - uv->x;
		}
		if (inverseV)
		{
			uv->y = 1 - uv->y;
		}

		//�i�[�p�f�[�^��p�ӂ��A�ǂݍ��񂾃��b�V��������������
		FBXVertex vertex = {};
		vertex.position = XMFLOAT3(position.x, position.y, position.z);
		vertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);
		vertex.uv = XMFLOAT2(uv->x, uv->y);
		vertex.tangent = XMFLOAT3(tangent->x, tangent->y, tangent->z);
		vertex.color = XMFLOAT4(color->r, color->g, color->b, color->a);

		//�x�N�g���Ɋi�[
		dst.vertices[i] = vertex;
	}

	//�C���f�b�N�X�������b�V�����~3�ɒ���
	dst.indices.resize(src->mNumFaces * 3);

	//�C���f�b�N�X���̊i�[
	for (auto i = 0u; i < src->mNumFaces; ++i)
	{
		const auto& face = src->mFaces[i];

		dst.indices[i * 3 + 0] = face.mIndices[0];
		dst.indices[i * 3 + 1] = face.mIndices[1];
		dst.indices[i * 3 + 2] = face.mIndices[2];
	}
}

/// <summary>
/// �e���b�V���ɕۑ�����Ă���{�[����ǂݍ��ފ֐�
/// </summary>
/// <param name="dst">�������ݐ�̃��b�V��</param>
/// <param name="src">�������݌��̃��b�V��</param>
/// <param name="boneMapping">�{�[���ƃ{�[�����̘A�z�z��</param>
/// <param name="boneInfos">�{�[�����</param>
void
AssimpLoader::LoadBone(Mesh& dst, const aiMesh* src, map<string, unsigned int>& boneMapping, vector<BoneInfo>& boneInfos)
{
	//���b�V�������e�{�[���f�[�^�ɑ΂��ȉ��̏������s��
	for (unsigned int i = 0; i < src->mNumBones; i++)							
	{
		//�C���f�b�N�X
		unsigned int boneIndex = 0;
		//�{�[�����
		aiBone* bone = src->mBones[i];												
		//�{�[����
		string boneName = bone->mName.data;										

		//�{�[�����E�{�[�����Ƀf�[�^���i�[���鏈��
		//�{�[�������o�^����Ă��Ȃ��ꍇ
		if (boneMapping.find(boneName) == boneMapping.end())
		{
			//�C���f�b�N�X���X�V������A�{�[�����ɉ��Z
			boneIndex = _boneNum;
			_boneNum++;

			//�{�[������o�^
			BoneInfo bi;														
			boneInfos.push_back(bi);
		}
		//���ɂ���ꍇ�C���f�b�N�X�����o�̃{�[���̂��̂ɍX�V
		else																	
		{
			boneIndex = boneMapping[boneName];
		}

		//�{�[���s��i���W���{�[����Ԃֈڂ��j���擾
		boneInfos[boneIndex]._boneOffset = LoadMatrix4x4(bone->mOffsetMatrix);	

		//�A�z�z��ɃC���f�b�N�X��o�^
		boneMapping[boneName] = boneIndex;

		//�{�[���Ɋi�[����Ă���E�F�C�g�ɑ΂����[�v
		for (UINT j = 0; j < bone->mNumWeights; j++)							
		{
			//�E�F�C�g�ɑΉ����钸�_�̃C���f�b�N�X�A�E�F�C�g���擾
			UINT id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;

			//���_�Ƀ{�[���ԍ��E�E�F�C�g���i�[
			SetIndexAndWeight(dst.vertices[id], boneIndex, weight);				
		}
	}
}

/// <summary>
/// FBX�t�@�C������e�N�X�`����ǂݍ��ފ֐�
/// </summary>
/// <param name="filename">FBX�t�@�C����</param>
/// <param name="dst">���b�V���z��</param>
/// <param name="src">�}�e���A��</param>
void
AssimpLoader::LoadTexture(const wchar_t* filename, Mesh& dst, const aiMaterial* src)
{
	aiString path;

	//�e�N�X�`���p�X��ǂݍ��߂��珈�������s
	if (src->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path) == AI_SUCCESS)	
	{
		//�e�N�X�`���p�X�͑��΃p�X�œ����Ă���̂ŁA�܂��f�B���N�g�������擾
		auto dir = GetDirectoryPath(filename);						
		//�t�@�C�������擾
		auto file = GetFilePath(path.C_Str());						
		//�Ō�Ƀf�B���N�g�����E�t�H���_���E�t�@�C�����������t����
		dst.diffuseMap = dir + L"textures/" + file;					
	}
	else
	{
		//�f�B�t���[�Y�}�b�v���N���A
		dst.diffuseMap.clear();										
	}
}

/// <summary>
/// ���_�Ƀ{�[��ID�A�E�F�C�g��ݒ肷��֐�
/// </summary>
/// <param name="vert">�������ݐ�̒��_</param>
/// <param name="id">�{�[��ID</param>
/// <param name="weight">�E�F�C�g</param>
void
AssimpLoader::SetIndexAndWeight(FBXVertex& vert, unsigned int id, float weight)
{
	//ID�̒������̉ӏ���T���A������ID�ƃE�F�C�g����������
	if (vert.ids.w == 0 && fabs(vert.weights.w) <= FLT_EPSILON)			
	{
		vert.ids.w = id;
		vert.weights.w = weight;
	}
	else if (vert.ids.x == 0 && fabs(vert.weights.x) <= FLT_EPSILON)
	{
		vert.ids.x = id;
		vert.weights.x = weight;
	}
	else if (vert.ids.y == 0 && fabs(vert.weights.y) <= FLT_EPSILON)
	{
		vert.ids.y = id;
		vert.weights.y = weight;
	}
	else if (vert.ids.z == 0 && fabs(vert.weights.z) <= FLT_EPSILON)
	{
		vert.ids.z = id;
		vert.weights.z = weight;
	}
}