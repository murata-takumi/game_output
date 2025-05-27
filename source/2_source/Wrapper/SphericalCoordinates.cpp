#include "Wrapper/SphericalCoordinates.h"

/// <summary>
/// �R���X�g���N�^(�����������ꍇ)
/// ���ɏ����͂��Ȃ�
/// </summary>
SphericalCoordinates::SphericalCoordinates()
{

}

/// <summary>
/// ���a��ݒ肷��֐�
/// </summary>
/// <param name="radius">�ݒ肷�锼�a</param>
void
SphericalCoordinates::SetRadius(float radius)
{
	_radius = radius;
}

/// <summary>
/// �p��ݒ肷��֐�
/// </summary>
/// <param name="elevation">�ݒ肷��p</param>
void
SphericalCoordinates::SetElevation(float elevation)
{
	_elevation = elevation;
}

/// <summary>
/// ���ʊp��ݒ肷��֐�
/// </summary>
/// <param name="elevation">�ݒ肷����ʊp</param>
void
SphericalCoordinates::SetAzimth(float azimth)
{
	_azimuth = azimth;
}

/// <summary>
/// ���a���擾����֐�
/// </summary>
/// <returns>�����t�����a</returns>
float
SphericalCoordinates::GetRadius()const
{
	return clamp(_radius, 50.0f, 5000.0f);
}

/// <summary>
/// �p���擾����֐�
/// </summary>
/// <returns>�����t���p</returns>
float
SphericalCoordinates::GetElevation()const
{
	return clamp(_elevation, (-89.9f + FLT_EPSILON) * (XM_PI / 180.0f), (89.9f - FLT_EPSILON) * (XM_PI / 180.0f));
}

/// <summary>
/// ���ʊp�A�p�𒲐�����֐�
/// </summary>
/// <param name="newAzimuth">���ʊp�ɉ��Z����l</param>
/// <param name="newElevation">�p�ɉ��Z����l</param>
/// <returns>���ʊp�A�p���������ꂽ�C���X�^���X</returns>
SphericalCoordinates
SphericalCoordinates::Rotate(float newAzimuth, float newElevation)
{
	newAzimuth = newAzimuth * (XM_PI / 180.0f);		//�x���@����ʓx�@�ɕϊ�
	newElevation = newElevation * (XM_PI / 180.0f);

	_azimuth += newAzimuth;							//�p�x�����Z
	_elevation = GetElevation() + newElevation;

	return *this;
}

/// <summary>
/// �J�����𒍎��_�ɋߕt����E��������֐�
/// </summary>
/// <param name="x">�����_����̋������猸�Z����l</param>
/// <returns>�����𒲐������C���X�^���X</returns>
SphericalCoordinates
SphericalCoordinates::Scaling(float x)
{
	_radius = GetRadius() - x;

	return *this;
}

/// <summary>
/// ���ʍ��W����ʏ���W�ɕϊ�����֐�
/// </summary>
/// <returns>�ʏ���W�̒l</returns>
Vector3
SphericalCoordinates::ToCartesian()
{
	float t = _radius * cos(GetElevation());												//�����_���王�_�ւ̋���(XZ����)
	return Vector3(t * sin(_azimuth), _radius * sin(GetElevation()), t * cos(_azimuth));	//���ʊp�E�p�ŉ��Z���ĕԂ�
}