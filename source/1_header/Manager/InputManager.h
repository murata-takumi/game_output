#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/Keyboard.h>
#include <DirectXTK12-master/Inc/Mouse.h>

/// <summary>
/// �L�[�{�[�h��}�E�X���̓��͂��Ǘ�����N���X
/// </summary>
class SpriteManager;
class InputManager
{
private:
	unique_ptr<Keyboard> _key;						//�L�[�{�[�h
	Keyboard::State _keyState;							//�L�[���
	Keyboard::KeyboardStateTracker _keyTracker;			//������Ă���{�^��

	unique_ptr<Mouse> _mouse;							//�}�E�X
	Mouse::State _mouseState;							//�}�E�X���
	Mouse::ButtonStateTracker _mouseTracker;			//�}�E�X��ԁi�{�^�����������E�������u�Ԃ��擾�j

public:
	InputManager();										//�R���X�g���N�^

	void UpdateInput();									//���͏����X�V����֐�

	Keyboard::State KeyState();							//�L�[��Ԃ��擾

	Keyboard::KeyboardStateTracker KeyTracker();		//�L�[��Ԃ��擾�i�L�[���������E�������u�Ԃ��擾�j

	Mouse::State MouseState();							//�}�E�X��Ԃ��擾

	Mouse::ButtonStateTracker MouseTracker();			//�}�E�X��Ԃ��擾�i�{�^�����������E�������u�Ԃ��擾�j
};
