//外部ライブラリをまとめたクラス
#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>
#include <DirectXTK12-master/Inc/SimpleMath.h>

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

const int DIFF = 15;
const int ANIMATION_HEIGHT = 200;