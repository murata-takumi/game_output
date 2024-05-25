#pragma once
#include "FBX/AnimNodes/Animations.h"
#include "Application.h"

/// <summary>
/// �A�j���[�V�������s���̊e�������܂Ƃ߂���ՃN���X
/// </summary>
class FBXActor;
class AnimNode
{
protected:
	//�A�N�^�[�̃|�C���^
	FBXActor* _actor;

	//���s����A�j���[�V�����̗񋓑�
	AnimEnum _anim;

public:

	//�R���X�g���N�^
	AnimNode(FBXActor* actor, AnimEnum anim);

	//���t���[���Ăяo��
	virtual void Update(float animTime);

	//�A�j���[�V�����J�n���ɌĂяo���֐�
	virtual void StartAnimation();

	//�A�j���[�V�����I�����ɌĂяo��
	virtual void EndAnimation();

	//�񋓑̂��擾����֐�
	AnimEnum GetAnimEnum()const;
};