#pragma once
#include "FBX/BoxCollider.h"
#include "Includes.h"

//正面ベクトルの初期値
const XMVECTOR INITIAL_FRONT = XMVectorSet(0.0f,0.0f,1.0f,0.0f);
//正面と右の角度の閾値
const float THRESHOLD_BETWEEN_FRONT_AND_RIGHT = 45.0f;
//右と背後の角度の閾値
const float THRESHOLD_BETWEEN_RIGHT_AND_BACK = 135.0f;
//背後と左の角度の閾値
const float THRESHOLD_BETWEEN_BACK_AND_LEFT = 225.0f;
//左と正面の角度の閾値
const float THRESHOLD_BETWEEN_LEFT_AND_FRONT = 315.0f;

/// <summary>
/// コンストラクタ
/// 当たり判定を構成する頂点を作成する
/// </summary>
/// <param name="size">当たり判定の幅、高さ、奥行のサイズ</param>
/// <param name="center">当たり判定の中心ベクトル</param>
BoxCollider::BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center)
{
	//ベクトルを初期化
	SetVec(INITIAL_FRONT);

	auto x = size.x;
	auto y = size.y;
	auto z = size.z;

	//半分の長さも取得
	_halfWidth = size.x / 2;
	_halfHeight = size.y / 2;
	_halfDepth = size.z / 2;

	//中心を初期化
	_initCenter = XMLoadFloat3(&center);
	auto centerX = center.x;
	auto centerY = center.y;
	auto centerZ = center.z;

	//頂点の初期化
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY - y / 2, centerZ - z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY - y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY - y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY - y / 2, centerZ - z / 2, 0.0f));

	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY + y / 2, centerZ - z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX - x / 2, centerY + y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY + y / 2, centerZ + z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(centerX + x / 2, centerY + y / 2, centerZ - z / 2, 0.0f));

	//頂点を代入
	_verts = _initVerts;
	_center = _initCenter;																				
}

/// <summary>
/// デストラクタ
/// </summary>
BoxCollider::~BoxCollider()
{

}

/// <summary>
/// 当たり判定が持つ三方向のベクトルを設定する関数
/// </summary>
/// <param name="vec">オブジェクトの正面ベクトル</param>
void 
BoxCollider::SetVec(const XMVECTOR& vec)
{
	//まずは正面ベクトル
	_frontDir = XMVector3Normalize(vec);

	//XZ平面に対し鉛直なベクトルをとり、それと正面ベクトルの外積を取り右ベクトルとする
	auto vertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_rightDir = XMVector3Normalize(XMVector3Cross(_frontDir, vertical));

	//正面ベクトル、右ベクトルの外積を上ベクトルとする
	_upDir = XMVector3Normalize(XMVector3Cross(_frontDir, _rightDir));
	//反転も忘れずに
	_upDir *= -1.0f;
}

/// <summary>
/// 毎フレームの更新を行う関数
/// 座標変換を行う
/// </summary>
/// <param name="mat">座標変換に用いる行列</param>
void
BoxCollider::Update(const XMMATRIX& mat)
{
	//スケーリング、回転、平行移動成分を行列から取得
	XMVECTOR scale, trans, skew;
	XMMatrixDecompose(&scale, &skew, &trans, mat);

	//X軸周りの回転を反転
	auto q = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f), XM_PI);
	skew = XMQuaternionMultiply(skew, q);						

	//スケーリング、回転、平行移動成分から新規ワールド行列を作成し、平行移動のX要素を反転
	XMMATRIX newWorldMat = XMMatrixScalingFromVector(scale) *						
		XMMatrixRotationQuaternion(skew) * XMMatrixTranslationFromVector(trans);
	newWorldMat.r[3].m128_f32[0] *= -1.0f;

	//正規化した正面ベクトルを取得
	auto inputVec = XMVectorSet(
		-1 * mat.r[2].m128_f32[0],
		mat.r[2].m128_f32[1],
		mat.r[2].m128_f32[2],
		0.0f
	);
	inputVec = XMVector3Normalize(inputVec);

	//OBBのベクトルを更新
	SetVec(inputVec);

	//中心も更新
	_center = XMVector3Transform(_initCenter, newWorldMat);

	//各初期座標に対し座標変換を行い、実際に表示する座標に代入
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], newWorldMat);
	}															
}

/// <summary>
/// 当たり判定を構成する頂点のベクトルを返す関数
/// </summary>
/// <returns>頂点のベクトル</returns>
vector<XMVECTOR>
BoxCollider::Vertices()const
{
	return _verts;
}

/// <summary>
/// 幅、高さ、奥行をまとめて返す関数
/// </summary>
/// <returns>幅、高さ、奥行</returns>
XMFLOAT3
BoxCollider::HalfLength()const
{
	return XMFLOAT3(_halfWidth,_halfHeight,_halfDepth);
}