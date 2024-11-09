#pragma once
#include "Application.h"

/// <summary>
/// �A�N�^�[�̃A�j���[�V���������肷�邽�߂̗񋓑�
/// </summary>
enum AnimEnum
{
	WAIT00,
	RUN00_F,
	JUMP00,
	FALL,
	Length,
};

/// <summary>
/// �񋓑̂ƃA�N�^�[�̃A�j���[�V�������̘A�z�z��
/// �񋓑̂��w�肵�ăA�j���[�V���������擾�ł���悤�ɂ��邽�߂��̂悤�Ȍ`�ɂ��Ă���
/// </summary>
static map<AnimEnum, string> animations
{
	{WAIT00,"WAIT00"},
	{RUN00_F,"RUN00_F"},
	{JUMP00,"JUMP00"},
	{FALL,"JUMP00B"},
};
