#include "FBX/BoxCollider2D.h"

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="size">当たり判定のサイズ</param>
/// <param name="center">中心座標</param>
BoxCollider2D::BoxCollider2D(const XMVECTOR& size, const XMVECTOR& center)
{
	_center = center;

	_halfWidth = size.m128_f32[0] / 2;
	_halfHeight = size.m128_f32[1] / 2;

	_rightDir = XMVectorSet(_halfWidth,0.0f,0.0f,0.0f);
	_upDir = XMVectorSet(0.0f,_halfHeight,0.0f, 0.0f);

	_rect = 
	{
		(LONG)(center.m128_f32[0] - size.m128_f32[0] / 2),
		(LONG)(center.m128_f32[1] - size.m128_f32[1] / 2),
		(LONG)(center.m128_f32[0] + size.m128_f32[0] / 2),
		(LONG)(center.m128_f32[1] + size.m128_f32[1] / 2),
	};
}

/// <summary>
/// デストラクタ
/// </summary>
BoxCollider2D::~BoxCollider2D()
{

}

/// <summary>
/// 矩形を返す
/// </summary>
/// <returns>矩形</returns>
RECT
BoxCollider2D::Rect()
{
	return _rect;
}

/// <summary>
/// 文字列を中央揃えにする
/// </summary>
/// <param name="str">文字列</param>
/// <returns>中央揃えになるような座標</returns>
XMFLOAT2
BoxCollider2D::StrCenterPos(string& str)
{
	//文字数を偶数にする
	if (str.length() % 2 != 0)
	{
		str = " " + str;
	}

	//矩形の中心から文字数×15だけ離す
	return XMFLOAT2(Rect().left + _halfWidth - str.length() * 15, Rect().top - 5);
}