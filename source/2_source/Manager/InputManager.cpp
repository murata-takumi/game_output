#include "Manager/InputManager.h"

/// <summary>
/// �R���X�g���N�^
/// </summary>
InputManager::InputManager()
{

}

/// <summary>
/// �f�X�g���N�^
/// </summary>
InputManager::~InputManager()
{

}

/// <summary>
/// �ÓI�ϐ���Ԃ��֐�
/// </summary>
/// <returns>�ÓI�ϐ�</returns>
InputManager&
InputManager::Instance()
{
	static InputManager instance;
	return instance;
}

/// <summary>
/// �������֐�
/// </summary>
void
InputManager::Init()
{
	//Keyboard�C���X�^���X��������
	_key = make_unique<Keyboard>();			
	//Mouse�C���X�^���X��������
	_mouse = make_unique<Mouse>();			

	//���͂��X�V
	UpdateInput();							
}

/// <summary>
/// ���͏����X�V����֐�
/// </summary>
void
InputManager::UpdateInput()
{
	//�L�[�{�[�h�̓��͂��X�V
	_keyState = _key->GetState();
	_keyTracker.Update(_keyState);

	//�}�E�X�̓��͂��X�V
	_mouseState = _mouse->GetState();	
	_mouseTracker.Update(_mouseState);
}

/// <summary>
/// �L�[��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�L�[���</returns>
Keyboard::State
InputManager::KeyState()
{
	return _keyState;
}

Keyboard::KeyboardStateTracker
InputManager::KeyTracker()
{
	return _keyTracker;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::State
InputManager::MouseState()
{
	return _mouseState;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�(�{�^�����������E�������u�Ԃ��擾)
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::ButtonStateTracker
InputManager::MouseTracker()
{
	return _mouseTracker;
}
