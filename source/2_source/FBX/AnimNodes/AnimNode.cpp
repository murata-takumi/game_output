#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FBXActor.h"

//�S�A�j���[�V�������ʂŎ��s���鏈��

/// <summary>
/// �R���X�g���N�^
/// �A�N�^�[�̃|�C���^�A�A�j���[�V�����ɑΉ�����񋓑̂��擾����
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
AnimNode::AnimNode(AnimEnum anim, const function<void(void)>& startFunc, const function<void(float)>& updateFunc, const function<void(void)>& endFunc)
	:_anim(anim),_startFunc(startFunc),_updateFunc(updateFunc),_endFunc(endFunc)
{

}

/// <summary>
/// �A�j���[�V�����J�n���ɌĂяo���֐�
/// �A�N�^�[�ɃA�j���[�V���������s������
/// </summary>
void
AnimNode::StartAnimation()
{
	//�R�[���o�b�N���s
	if (_startFunc != nullptr)
	{
		_startFunc();
	}
};

/// <summary>
/// �A�j���[�V�������s���A���t���[���Ăяo���֐�
/// </summary>
/// <param name="animTime">�A�j���[�V�����̍Đ�����</param>
void 
AnimNode::Update(float& animTime)
{
	//�R�[���o�b�N���s
	if (_updateFunc != nullptr)
	{
		_updateFunc(animTime);
	}
};

/// <summary>
/// �A�j���[�V�����I�����ɌĂяo���֐�
/// </summary>
void 
AnimNode::EndAnimation()
{
	//�R�[���o�b�N���s
	if (_endFunc != nullptr)
	{
		_endFunc();
	}
};

/// <summary>
/// �񋓑̂�Ԃ��֐�
/// </summary>
/// <returns>�񋓑�</returns>
AnimEnum
AnimNode::GetAnimEnum()const
{
	return _anim;
}