#pragma once
#include "Includes.h"
#include "Vector3.h"

/// <summary>
/// カメラ座標を注視点を中心とした球体として管理するためのクラス
/// </summary>
class SphericalCoordinates
{
public:
	//半径を取得する関数
	float GetRadius()const;												

	//半径を設定する関数
	void SetRadius(float radius);										
	//仰角を設定する関数
	void SetElevation(float elevation);									
	//方位角を設定する関数
	void SetAzimth(float azimth);										

	//コンストラクタ
	SphericalCoordinates();												

	//カメラを回転させる関数
	SphericalCoordinates Rotate(float newAzimuth, float newElevation);	
	//カメラを注視点に近付ける・遠ざける関数
	SphericalCoordinates Scaling(float x);								

	//球面座標から通常座標に変換する関数
	Vector3 ToCartesian();												

private:
	//それぞれ半径・方位角・仰角
	float _radius, _azimuth, _elevation;							

	//仰角を取得する関数
	float GetElevation()const;										
};
