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

	Vector3()
		:_data(XMFLOAT3(0,0,0))
	{}
	Vector3(float fx, float fy, float fz)
		:_data(XMFLOAT3(fx, fy, fz))
	{}
	Vector3(const Vector3& vec3)
		:_data(vec3)
	{}
	Vector3(const XMFLOAT3& f)
		: _data(f)
	{}
	Vector3(const XMVECTOR& v)
		: _data(XMFLOAT3(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2]))
	{}

	float& X()
	{
		return _data.x;
	}
	float& Y()
	{
		return _data.y;
	}
	float& Z()
	{
		return _data.z;
	}

	const float& X()const
	{
		return _data.x;
	}
	const float& Y()const
	{
		return _data.y;
	}
	const float& Z()const
	{
		return _data.z;
	}

	operator XMFLOAT3() const
	{
		return _data;
	}

	operator XMVECTOR() const
	{
		return XMLoadFloat3(&_data);
	}

	float& operator[](const int& idx)
	{
		return XMLoadFloat3(&_data).m128_f32[idx];
	}

	XMVECTOR* operator&()
	{
		XMVECTOR ret = XMLoadFloat3(&_data);
		return &ret;
	}

	Vector3& operator=(const Vector3& other)
	{
		_data = other._data;

		return *this;
	}

	Vector3& operator=(const XMVECTOR& other)
	{
		XMStoreFloat3(&_data, other);

		return *this;
	}

	Vector3& operator=(const XMFLOAT3& other)
	{
		_data = other;

		return *this;
	}

	Vector3& operator+=(const XMVECTOR& other)
	{
		_data.x += other.m128_f32[0];
		_data.y += other.m128_f32[1];
		_data.z += other.m128_f32[2];

		return *this;
	}

	Vector3& operator*=(const float& other)
	{
		_data.x *= other;
		_data.y *= other;
		_data.z *= other;

		return *this;
	}
};