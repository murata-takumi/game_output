#pragma once
#include "Collider/BoxCollider.h"
#include "FBX/FBXBase.h"
#include "Includes.h"

//正面ベクトルの初期値
const Vector3 INITIAL_FRONT = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

/// <summary>
/// コンストラクタ
/// 当たり判定を構成する頂点を作成する
/// </summary>
/// <param name="size">当たり判定の幅、高さ、奥行のサイズ</param>
/// <param name="center">当たり判定の中心ベクトル</param>
BoxCollider::BoxCollider(FBXBase& object, const Vector3& size)
	:_object(object)
{
	//ベクトルを初期化
	SetVec(INITIAL_FRONT);

	auto x = size.X();
	auto y = size.Y();
	auto z = size.Z();

	//半分の長さも取得
	_halfWidth = size.X() / 2;
	_halfHeight = size.Y() / 2;
	_halfDepth = size.Z() / 2;

	//中心を初期化
	_initCenter = XMVectorSet(0,0,0,0);

	//頂点の初期化
	_initVerts.emplace_back(XMVectorSet(-x / 2, -y / 2, -z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(-x / 2, -y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, -y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, -y / 2, -z / 2, 0.0f));

	_initVerts.emplace_back(XMVectorSet(-x / 2, y / 2, -z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(-x / 2, y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, y / 2, z / 2, 0.0f));
	_initVerts.emplace_back(XMVectorSet(x / 2, y / 2, -z / 2, 0.0f));

	//頂点を代入
	_center = _initCenter;
	_verts = _initVerts;
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
BoxCollider::SetVec(const Vector3& vec)
{
	//まずは正面ベクトル
	_frontDir = XMVector3Normalize(vec);

	//XZ平面に対し鉛直なベクトルをとり、それと正面ベクトルの外積を取り右ベクトルとする
	auto vertical = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_rightDir = XMVector3Normalize(XMVector3Cross(_frontDir, vertical));

	//正面ベクトル、右ベクトルの外積を上ベクトルとする
	_upDir = XMVector3Normalize(XMVector3Cross(_frontDir, _rightDir));
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
	_center = XMVector3Transform(_initCenter, mat);

	//各初期座標に対し座標変換を行い、実際に表示する座標に代入
	for (int i = 0; i < _initVerts.size(); i++)	
	{
		_verts[i] = XMVector3Transform(_initVerts[i], mat);
	}															
}

/// <summary>
/// 当たり判定を構成する頂点のベクトルを返す関数
/// </summary>
/// <returns>頂点のベクトル</returns>
vector<Vector3>
BoxCollider::Vertices()const
{
	return _verts;
}

/// <summary>
/// 幅、高さ、奥行をまとめて返す関数
/// </summary>
/// <returns>幅、高さ、奥行</returns>
Vector3
BoxCollider::HalfLength()const
{
	return Vector3(_halfWidth,_halfHeight,_halfDepth);
}

/// <summary>
/// 中心座標を返す関数
/// </summary>
/// <returns></returns>
Vector3
BoxCollider::Center()const
{
	return _center;
}

/// <summary>
/// 当たり判定を持つオブジェクトを返す
/// </summary>
/// <returns>オブジェクト</returns>
FBXBase&
BoxCollider::Object()const
{
	return _object;
}