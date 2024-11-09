#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FBXActor.h"

//�S�A�j���[�V�������ʂŎ��s���鏈��

/// <summary>
/// �R���X�g���N�^
/// �A�N�^�[�̃|�C���^�A�A�j���[�V�����ɑΉ�����񋓑̂��擾����
/// </summary>
/// <param name="actor">�|�C���^</param>
/// <param name="anim">�񋓑�</param>
AnimNode::AnimNode(FBXActor* actor, AnimEnum anim)
	:_actor(actor), _anim(anim)
{

}

/// <summary>
/// �A�j���[�V�����J�n���ɌĂяo���֐�
/// �A�N�^�[�ɃA�j���[�V���������s������
/// </summary>
void
AnimNode::StartAnimation()
{
	_actor->BlendAnimation(_anim);
};

/// <summary>
/// �A�j���[�V�������s���A���t���[���Ăяo���֐�
/// </summary>
/// <param name="animTime">�A�j���[�V�����̍Đ�����</param>
void 
AnimNode::Update(float& animTime)
{

};

/// <summary>
/// �A�j���[�V�����I�����ɌĂяo���֐�
/// </summary>
void 
AnimNode::EndAnimation()
{

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