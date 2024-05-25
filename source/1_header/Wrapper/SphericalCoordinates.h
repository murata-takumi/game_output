#pragma once
#include "Application.h"

/// <summary>
/// �J�������W�𒍎��_�𒆐S�Ƃ������̂Ƃ��ĊǗ����邽�߂̃N���X
/// </summary>
class SphericalCoordinates
{
public:
	float GetRadius()const;												//���a���擾����֐�

	void SetRadius(float radius);										//���a��ݒ肷��֐�
	void SetElevation(float elevation);									//�p��ݒ肷��֐�
	void SetAzimth(float azimth);										//���ʊp��ݒ肷��֐�

	SphericalCoordinates();												//�R���X�g���N�^

	SphericalCoordinates Rotate(float newAzimuth, float newElevation);	//�J��������]������֐�
	SphericalCoordinates Scaling(float x);								//�J�����𒍎��_�ɋߕt����E��������֐�

	XMFLOAT3 ToCartesian();												//���ʍ��W����ʏ���W�ɕϊ�����֐�

private:
	float _radius, _azimuth, _elevation;							//���ꂼ�ꔼ�a�E���ʊp�E�p

	float GetElevation()const;										//�p���擾����֐�
};
