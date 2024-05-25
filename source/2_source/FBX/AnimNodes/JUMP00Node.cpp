#pragma once
#include "FBX/AnimNodes/JUMP00Node.h"
#include "FBX/FBXActor.h"

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
/// ���t���[���X�V����֐�
/// </summary>
/// <param name="animTime">���݂̌o�ߎ���</param>
void
JUMP00Node::Update(float animTime)
{
	AnimNode::Update(animTime);

	//��莞�Ԍo�߂�����A�j���[�V�����J�ڂł���悤�ɂ���
	if (animTime >= 35.0f)
	{
		_actor->SetCanChangeAnim(true);
		_actor->SetOnGround(true);
	}
}

/// <summary>
/// �A�j���[�V�����J�n���ɌĂяo���֐�
/// </summary>
void
JUMP00Node::StartAnimation()
{
	AnimNode::StartAnimation();

	//�A�j���[�V�������x��ݒ�
	_actor->SetAnimationTime(8.5f);
	//�ʃA�j���[�V�����ɑJ�ڂ��Ȃ��悤�ɂ���
	_actor->SetCanChangeAnim(false);
	//�󒆂ɂ���Ɛݒ�
	_actor->SetOnGround(false);
	//���[�v���Ȃ��悤�ɂ���
	_actor->SetIsInLoop(false);
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