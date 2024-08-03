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
/// FBXモデルを構成するメッシュの構造体
/// </summary>
struct Mesh
{
	//頂点ベクトル
	vector<FBXVertex> vertices;
	//インデックスベクトル
	vector<uint32_t> indices;
	//テクスチャのファイルパス
	wstring diffuseMap;
};

/// <summary>
/// ボーンそのものが持つ情報
/// </summary>
struct BoneInfo
{
	//頂点を回転の中心へ移動させる行列
	XMMATRIX _boneOffset;
	//最終的な回転行列
	XMMATRIX _finalTrans;
};

/// <summary>
/// モデルデータを受け取る構造体
/// </summary>
struct ImportSettings
{
	//出力先のメッシュベクトル
	vector<Mesh>& meshes;
	//ボーン名とインデックスの連想配列
	map<string, unsigned int>& boneMapping;
	//ボーン情報
	vector<BoneInfo>& boneInfos;
	//U座標を反転させるか
	bool inverseU = false;
	//V座標を反転させるか
	bool inverseV = false;
};

class AssimpLoader
{
public:
	//インスタンス取得関数
	static AssimpLoader& Instance();

	//シーンデータをロードする関数
	const aiScene* LoadScene(const wchar_t* filePath);

	//モデルをロードする関数
	void LoadModel(const aiScene* scene, const wchar_t* filePath,		
		ImportSettings& import);

private:
	//ボーン数
	unsigned int _boneNum = 0;

	//コンストラクタ
	AssimpLoader();
	AssimpLoader(const AssimpLoader&) = delete;
	//デストラクタ
	~AssimpLoader();

	//メッシュをロードする関数
	void LoadMesh(Mesh& dst, const aiMesh* src, bool inverseU, bool inverseV);
	//メッシュ内のボーンをロードする関数
	void LoadBone(Mesh& dst, const aiMesh* src,										
		map<string, unsigned int>& boneMapping,vector<BoneInfo>& boneInfos);
	//テクスチャをロードする関数
	void LoadTexture(const wchar_t* filename,Mesh& dst,const aiMaterial* src);		

	//頂点にボーンIDとウェイトを設定する関数
	void SetIndexAndWeight(FBXVertex& vert,unsigned int id,float weight);			

	//モデルデータインポート用オブジェクト
	Assimp::Importer _importer;														
};