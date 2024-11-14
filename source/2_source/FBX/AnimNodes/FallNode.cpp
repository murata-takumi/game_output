#pragma once
#include "FBX/AnimNodes/FallNode.h"
#include "FBX/FBXActor.h"

//���[�v�̊J�n�A�I������
const float LOOP_BEGIN_TIME = 30.0f;
const float LOOP_END_TIME = 31.0f;
//�A�j���[�V�������x
const float ANIM_SPEED = 0.1f;

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
	_actor->SetAnimationSpeed(ANIM_SPEED);
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
	if (animTime <= LOOP_BEGIN_TIME)
	{
		animTime = LOOP_BEGIN_TIME;
		_actor->SetAnimationSpeed(ANIM_SPEED);
	}
	else if (LOOP_END_TIME < animTime)
	{
		animTime = LOOP_END_TIME;
		_actor->SetAnimationSpeed(-ANIM_SPEED);
	}

	//�n�ʂɒ�������ʃA�j���[�V�����ɑJ�ڂł���悤�ɂ���
	if (_actor->GetOnGround())
	{
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// �A�j���[�V�����I�����ɌĂяo���֐�
/// </summary>
void
FallNode::EndAnimation()
{
	//�A�j���[�V�������x�����ɖ߂�
	_actor->SetAnimationSpeed(1.0f);
}