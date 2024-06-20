#pragma once
#include "FBX/BoxCollider.h"
#include "Includes.h"

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
/// <param name="x">X幅</param>
/// <param name="y">Y高さ</param>
/// <param name="z">Z幅</param>
/// <param name="center">当たり判定の中心ベクトル</param>
BoxCollider::BoxCollider(const XMFLOAT3& size, const XMFLOAT3& center)
{
	auto x = size.x;
	auto y = size.y;
	auto z = size.z;

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

	//境界値を初期化
	SetBoundValues();																					
}

/// <summary>
/// デストラクタ
/// </summary>
BoxCollider::~BoxCollider()
{

}

/// <summary>
/// 当たり判定に必要な境界値を設定する関数
/// </summary>
void
BoxCollider::SetBoundValues()
{
	//X、Z座標の境界値
	//現在のオブジェクトの角度に応じて場合分けする
	//下方向
	if(THRESHOLD_BETWEEN_LEFT_AND_FRONT <= _angle || _angle < THRESHOLD_BETWEEN_FRONT_AND_RIGHT)
	{
		_boundValues[BOX_COL_X_MIN] = _verts[1].m128_f32[0];
		_boundValues[BOX_COL_X_MAX] = _verts[3].m128_f32[0];

		_boundValues[BOX_COL_Z_MIN] = _verts[1].m128_f32[2];
		_boundValues[BOX_COL_Z_MAX] = _verts[3].m128_f32[2];
	}
	//右方向
	else if (THRESHOLD_BETWEEN_FRONT_AND_RIGHT <= _angle && _angle < THRESHOLD_BETWEEN_RIGHT_AND_BACK)
	{
		_boundValues[BOX_COL_X_MIN] = _verts[1].m128_f32[0];
		_boundValues[BOX_COL_X_MAX] = _verts[3].m128_f32[0];

		_boundValues[BOX_COL_Z_MIN] = _verts[2].m128_f32[2];
		_boundValues[BOX_COL_Z_MAX] = _verts[0].m128_f32[2];
	}
	//上方向
	else if (THRESHOLD_BETWEEN_RIGHT_AND_BACK <= _angle && _angle < THRESHOLD_BETWEEN_BACK_AND_LEFT)
	{
		_boundValues[BOX_COL_X_MIN] = _verts[2].m128_f32[0];
		_boundValues[BOX_COL_X_MAX] = _verts[0].m128_f32[0];

		_boundValues[BOX_COL_Z_MIN] = _verts[3].m128_f32[2];
		_boundValues[BOX_COL_Z_MAX] = _verts[1].m128_f32[2];
	}
	//左方向
	else if (THRESHOLD_BETWEEN_BACK_AND_LEFT <= _angle && _angle < THRESHOLD_BETWEEN_LEFT_AND_FRONT)
	{
		_boundValues[BOX_COL_X_MIN] = _verts[0].m128_f32[0];
		_boundValues[BOX_COL_X_MAX] = _verts[2].m128_f32[0];

		_boundValues[BOX_COL_Z_MIN] = _verts[0].m128_f32[2];
		_boundValues[BOX_COL_Z_MAX] = _verts[2].m128_f32[2];
	}

	//Y座標の境界値
	_boundValues[BOX_COL_Y_MIN] = _verts[0].m128_f32[1];
	_boundValues[BOX_COL_Y_MAX] = _verts[4].m128_f32[1];
}

/// <summary>
/// 毎フレームの更新を行う関数
/// 座標変換を行う
/// </summary>
/// <param name="mat">座標変換に用いる行列</param>
/// <param name="angle">オブジェクトの角度</param>
void
BoxCollider::Update(const XMMATRIX& mat, float angle)
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

	auto inputVec = XMVectorSet(
		mat.r[2].m128_f32[0],
		mat.r[2].m128_f32[1],
		mat.r[2].m128_f32[2],
		0.0f
	);
	inputVec = XMVector3Normalize(inputVec);

	//オブジェクトの角度を保存
	_angle = angle;

	//各初期座標に対し座標変換を行い、実際に表示する座標に代入
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], newWorldMat);
	}

	//境界値を更新
	SetBoundValues();																
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
/// 当たり判定の各座標を取得する関数
/// </summary>
/// <returns>各座標のベクトル</returns>
map<string, float>
BoxCollider::BoundValues()const
{
	return _boundValues;
}