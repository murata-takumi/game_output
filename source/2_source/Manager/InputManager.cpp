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
/// �V���O���g����Ԃ��֐�
/// </summary>
/// <returns>�V���O���g��</returns>
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
/// �}�E�X���W��Ԃ��֐�
/// </summary>
/// <returns>���W</returns>
Pos2D
InputManager::MousePos()const
{
	Pos2D ret;
	ret._posX = _mouseState.x;
	ret._posY = _mouseState.y;

	return ret;
}

/// <summary>
/// �L�[��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�L�[���</returns>
Keyboard::State
InputManager::KeyState()const
{
	return _keyState;
}

Keyboard::KeyboardStateTracker
InputManager::KeyTracker()const
{
	return _keyTracker;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::State
InputManager::MouseState()const
{
	return _mouseState;
}

/// <summary>
/// �}�E�X��Ԃ�Ԃ��֐�(�{�^�����������E�������u�Ԃ��擾)
/// </summary>
/// <returns>�}�E�X���</returns>
Mouse::ButtonStateTracker
InputManager::MouseTracker()const
{
	return _mouseTracker;
}
