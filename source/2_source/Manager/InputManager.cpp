#include "Manager/InputManager.h"

/// <summary>
/// コンストラクタ
/// </summary>
InputManager::InputManager()
{

}

/// <summary>
/// デストラクタ
/// </summary>
InputManager::~InputManager()
{

}

/// <summary>
/// 静的変数を返す関数
/// </summary>
/// <returns>静的変数</returns>
InputManager&
InputManager::Instance()
{
	static InputManager instance;
	return instance;
}

/// <summary>
/// 初期化関数
/// </summary>
void
InputManager::Init()
{
	//Keyboardインスタンスを初期化
	_key = make_unique<Keyboard>();			
	//Mouseインスタンスを初期化
	_mouse = make_unique<Mouse>();			

	//入力を更新
	UpdateInput();							
}

/// <summary>
/// 入力情報を更新する関数
/// </summary>
void
InputManager::UpdateInput()
{
	//キーボードの入力を更新
	_keyState = _key->GetState();
	_keyTracker.Update(_keyState);

	//マウスの入力を更新
	_mouseState = _mouse->GetState();	
	_mouseTracker.Update(_mouseState);
}

/// <summary>
/// キー状態を返す関数
/// </summary>
/// <returns>キー状態</returns>
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
/// マウス状態を返す関数
/// </summary>
/// <returns>マウス状態</returns>
Mouse::State
InputManager::MouseState()
{
	return _mouseState;
}

/// <summary>
/// マウス状態を返す関数(ボタンを押した・離した瞬間を取得)
/// </summary>
/// <returns>マウス状態</returns>
Mouse::ButtonStateTracker
InputManager::MouseTracker()
{
	return _mouseTracker;
}
