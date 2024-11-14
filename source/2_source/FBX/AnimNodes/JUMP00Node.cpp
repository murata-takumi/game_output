#pragma once
#include "FBX/AnimNodes/JUMP00Node.h"
#include "FBX/FBXActor.h"

//�A�j���[�V�����̊J�n����
const float START_TIME = 8.5f;
//�ʃA�j���[�V�����ւ̑J�ڂ��o����悤�ɂȂ鎞��
const float TRANSITION_TIME = 35.0f;

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
JUMP00Node::JUMP00Node(FBXActor* actor, AnimEnum anim)
	:AnimNode(actor, anim)
{

}

/// <summary>
/// �A�j���[�V�����J�n���ɌĂяo���֐�
/// </summary>
void
JUMP00Node::StartAnimation()
{
	AnimNode::StartAnimation();

	//�J�n���Ԃ�������ɐݒ�
	_actor->SetAnimationTime(START_TIME);
	//�ʃA�j���[�V�����ɑJ�ڂ��Ȃ��悤�ɂ���
	_actor->SetCanChangeAnim(false);
	//���[�v���Ȃ��悤�ɂ���
	_actor->SetIsInLoop(false);
}

/// <summary>
/// ���t���[���X�V����֐�
/// </summary>
/// <param name="animTime">���݂̌o�ߎ���</param>
void
JUMP00Node::Update(float& animTime)
{
	AnimNode::Update(animTime);

	//��莞�Ԍo�߂�����A�j���[�V�����J�ڂł���悤�ɂ���
	if (animTime >= TRANSITION_TIME)
	{
		_actor->SetCanChangeAnim(true);
	}
}

/// <summary>
/// �A�j���[�V�����I�����ɌĂяo���֐�
/// </summary>
void
JUMP00Node::EndAnimation()
{
	//���[�v�ł���悤�ɂ���
	_actor->SetIsInLoop(true);
}