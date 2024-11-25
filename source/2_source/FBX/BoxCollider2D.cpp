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