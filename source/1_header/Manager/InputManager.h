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
	//�L�[�{�[�h
	unique_ptr<Keyboard> _key;	
	//�L�[���
	Keyboard::State _keyState;						
	//������Ă���{�^��
	Keyboard::KeyboardStateTracker _keyTracker;			

	//�}�E�X
	unique_ptr<Mouse> _mouse;							
	//�}�E�X���
	Mouse::State _mouseState;							
	//�}�E�X��ԁi�{�^�����������E�������u�Ԃ��擾�j
	Mouse::ButtonStateTracker _mouseTracker;	

	//�R���X�g���N�^
	InputManager();						
	InputManager(const InputManager&) = delete;
	//�f�X�g���N�^
	~InputManager();

public:
	//�V���O���g����Ԃ�
	static InputManager& Instance();

	//������
	void Init();

	//���͏����X�V����֐�
	void UpdateInput();									

	//�L�[��Ԃ��擾
	Keyboard::State KeyState();							

	//�L�[��Ԃ��擾�i�L�[���������E�������u�Ԃ��擾�j
	Keyboard::KeyboardStateTracker KeyTracker();		

	//�}�E�X��Ԃ��擾
	Mouse::State MouseState();							

	//�}�E�X��Ԃ��擾�i�{�^�����������E�������u�Ԃ��擾�j
	Mouse::ButtonStateTracker MouseTracker();			
};
