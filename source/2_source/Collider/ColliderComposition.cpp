#include "Vector3.h"

#include "Collider/ColliderComposition.h"
#include "Fbx/IFbx.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="pos">���W</param>
/// <param name="object">�R�t������I�u�W�F�N�g</param>
ColliderComposition::ColliderComposition(const Vector3& pos, IFbx* object)
{
	//�n���ꂽ�A�h���X����ɃX�}�[�g�|�C���^�𐶐�
	//�Ǘ������X�}�[�g�|�C���^�ɓn������deleter?��n��
	_object = shared_ptr<IFbx>(object, [](IFbx*)
	{

	});

	//���S��������
	_initCenter = make_shared<Vector3>(pos);
	_center = make_shared<Vector3>(*_initCenter);
}

void 
ColliderComposition::Update(const XMMATRIX& mat)
{
	//���S���W���X�V
	*_center = XMVector3Transform(*_initCenter, mat);
}

/// <summary>
/// ���S���W��Ԃ��֐�
/// </summary>
/// <returns></returns>
shared_ptr <Vector3>
ColliderComposition::Center()const
{
	return _center;
}

/// <summary>
/// �����蔻������I�u�W�F�N�g��Ԃ�
/// </summary>
/// <returns>�I�u�W�F�N�g</returns>
IFbx& 
ColliderComposition::Object()const
{
	return *_object;
}