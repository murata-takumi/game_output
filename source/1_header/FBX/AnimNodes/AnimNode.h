#pragma once
#include "Includes.h"

#include "FBX/AnimNodes/Animations.h"

/// <summary>
/// �A�j���[�V�������s���̊e�������܂Ƃ߂���ՃN���X
/// </summary>
class AnimNode
{
private:

	//���s����A�j���[�V�����̗񋓑�
	AnimEnum _anim;

	//���ꂼ��A�j���[�V�����J�n���A�X�V���A�I�����Ɏ��s����R�[���o�b�N
	function<void(void)> _startFunc;
	function<void(float)> _updateFunc;
	function<void(void)> _endFunc;

public:

	//�R���X�g���N�^
	AnimNode(AnimEnum anim, const function<void(void)>& startFunc = nullptr, const function<void(float)>& updateFunc = nullptr, const function<void(void)>& endFunc = nullptr);


	//�A�j���[�V�����J�n���ɌĂяo���֐�
	void StartAnimation();

	//���t���[���Ăяo��
	void Update(float& animTime);

	//�A�j���[�V�����I�����ɌĂяo��
	void EndAnimation();

	//�񋓑̂��擾����֐�
	AnimEnum GetAnimEnum()const;
};