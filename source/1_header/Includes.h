//外部ライブラリをまとめたクラス
#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>
#include <DirectXTK12-master/Inc/SimpleMath.h>
#include <DirectXMath.h>

#include <d3dcompiler.h>
#include <array>
#include <cstdlib>
#include <map>
#include <tchar.h>
#include <dxgi1_6.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

const XMVECTOR Y_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);	
const XMVECTOR Z_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

const string BOX_COL_X_MIN = "XMin";
const string BOX_COL_X_MAX = "XMax";
const string BOX_COL_Y_MIN = "YMin";
const string BOX_COL_Y_MAX = "YMax";
const string BOX_COL_Z_MIN = "ZMin";
const string BOX_COL_Z_MAX = "ZMax";

//更新回数を60FPSに固定する際の1フレームの秒数
const float FRAME_TIME = 1.0f / 60.0f;

const int DIFF = 15;
const int ANIMATION_HEIGHT = 200;

/// <summary>
/// マウス座標をまとめた構造体
/// </summary>
struct Pos2D
{
	float _posX;
	float _posY;
};

/// <summary>
/// FBXモデルの頂点用構造体
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//座標
	XMFLOAT3 normal;					//法線
	XMFLOAT2 uv;						//UV座標
	XMFLOAT3 tangent;					//正接
	XMFLOAT4 color;						//頂点カラー
	XMUINT4 ids;						//ボーンID
	XMFLOAT4 weights;					//ボーンの影響値
};

/// <summary>
/// オブジェクトが持つ当たり判定の型を決める列挙値
/// </summary>
enum ColliderType
{
	Box,
	Capsule,
	Sphere,
};