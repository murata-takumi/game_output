#pragma once
#include "FBX/AnimNodes/FallNode.h"
#include "FBX/FBXActor.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
FallNode::FallNode(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}

/// <summary>
/// �A�j���[�V�����J�n���ɌĂяo���֐�
/// </summary>
void
FallNode::StartAnimation()
{
	AnimNode::StartAnimation();

	//�ʃA�j���[�V�����ɑJ�ڂ��Ȃ��悤�ɂ���
	_actor->SetCanChangeAnim(false);
	_actor->SetAnimationSpeed(0.1f);
}

/// <summary>
/// ���t���[���X�V����֐�
/// </summary>
/// <param name="animTime">���݂̌o�ߎ���</param>
void
FallNode::Update(float& animTime)
{
	AnimNode::Update(animTime);

	//���[�v���Ă���悤�Ɍ�����ׂ̋���̍�
	if (animTime <= 30.0f)
	{
		animTime = 30.0f;
		_actor->SetAnimationSpeed(0.1f);
	}
	else if (31.0f < animTime)
	{
		animTime = 31.0f;
		_actor->SetAnimationSpeed(-0.1f);
	}

	if (_actor->GetOnGround())
	{
		//�ʃA�j���[�V�����ɑJ�ڂł���悤�ɂ���
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// �A�j���[�V�����I�����ɌĂяo���֐�
/// </summary>
void
FallNode::EndAnimation()
{
	_actor->SetAnimationSpeed(1.0f);
}