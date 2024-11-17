//�O�����C�u�������܂Ƃ߂��N���X
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

//�X�V�񐔂�60FPS�ɌŒ肷��ۂ�1�t���[���̕b��
const float FRAME_TIME = 1.0f / 60.0f;

const int DIFF = 15;
const int ANIMATION_HEIGHT = 200;

/// <summary>
/// �}�E�X���W���܂Ƃ߂��\����
/// </summary>
struct Pos2D
{
	float _posX;
	float _posY;
};

/// <summary>
/// FBX���f���̒��_�p�\����
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//���W
	XMFLOAT3 normal;					//�@��
	XMFLOAT2 uv;						//UV���W
	XMFLOAT3 tangent;					//����
	XMFLOAT4 color;						//���_�J���[
	XMUINT4 ids;						//�{�[��ID
	XMFLOAT4 weights;					//�{�[���̉e���l
};