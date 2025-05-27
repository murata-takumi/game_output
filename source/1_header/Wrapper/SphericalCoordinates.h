#pragma once
#include "Includes.h"
#include "Vector3.h"

/// <summary>
/// �J�������W�𒍎��_�𒆐S�Ƃ������̂Ƃ��ĊǗ����邽�߂̃N���X
/// </summary>
class SphericalCoordinates
{
public:
	//���a���擾����֐�
	float GetRadius()const;												

	//���a��ݒ肷��֐�
	void SetRadius(float radius);										
	//�p��ݒ肷��֐�
	void SetElevation(float elevation);									
	//���ʊp��ݒ肷��֐�
	void SetAzimth(float azimth);										

	//�R���X�g���N�^
	SphericalCoordinates();												

	//�J��������]������֐�
	SphericalCoordinates Rotate(float newAzimuth, float newElevation);	
	//�J�����𒍎��_�ɋߕt����E��������֐�
	SphericalCoordinates Scaling(float x);								

	//���ʍ��W����ʏ���W�ɕϊ�����֐�
	Vector3 ToCartesian();												

private:
	//���ꂼ�ꔼ�a�E���ʊp�E�p
	float _radius, _azimuth, _elevation;							

	//�p���擾����֐�
	float GetElevation()const;										
};
