#pragma once
#include "Application.h"

/// <summary>
/// XMVector,XMFloat3の両方を扱うクラス
/// </summary>
class Vector3
{
private:
	//実データ
	XMFLOAT3 _data;

public:
	//コンストラクタ、渡されたデータの型に応じて処理を変える
	Vector3()
	{
		_data = XMFLOAT3(0, 0, 0);
	}
	Vector3(float x, float y, float z)
	{
		_data = XMFLOAT3(x, y, z);
	}
	Vector3(const XMFLOAT3& f)
	{
		_data = f;
	}
	Vector3(const XMVECTOR& vec)
	{
		XMStoreFloat3(&_data,vec);
	}

	operator XMFLOAT3() const
	{
		return _data;
	}

	operator XMVECTOR() const
	{
		return XMLoadFloat3(&_data);
	}

	Vector3& operator =(XMVECTOR other)
	{
		_data.x = other.m128_f32[0];
		_data.y = other.m128_f32[1];
		_data.z = other.m128_f32[2];

		return *this;
	}

	Vector3& operator=(XMFLOAT3 other)
	{
		_data.x = other.x;
		_data.y = other.y;
		_data.z = other.z;

		return *this;
	}

	Vector3& operator+=(XMVECTOR other)
	{
		_data.x += other.m128_f32[0];
		_data.y += other.m128_f32[1];
		_data.z += other.m128_f32[2];

		return *this;
	}
};