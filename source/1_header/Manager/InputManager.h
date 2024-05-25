#pragma once
#include "Application.h"

#include <DirectXTK12-master/Inc/Keyboard.h>
#include <DirectXTK12-master/Inc/Mouse.h>

/// <summary>
/// キーボードやマウス等の入力を管理するクラス
/// </summary>
class SpriteManager;
class InputManager
{
private:
	unique_ptr<Keyboard> _key;						//キーボード
	Keyboard::State _keyState;							//キー状態
	Keyboard::KeyboardStateTracker _keyTracker;			//押されているボタン

	unique_ptr<Mouse> _mouse;							//マウス
	Mouse::State _mouseState;							//マウス状態
	Mouse::ButtonStateTracker _mouseTracker;			//マウス状態（ボタンを押した・離した瞬間を取得）

public:
	InputManager();										//コンストラクタ

	void UpdateInput();									//入力情報を更新する関数

	Keyboard::State KeyState();							//キー状態を取得

	Keyboard::KeyboardStateTracker KeyTracker();		//キー状態を取得（キーを押した・離した瞬間を取得）

	Mouse::State MouseState();							//マウス状態を取得

	Mouse::ButtonStateTracker MouseTracker();			//マウス状態を取得（ボタンを押した・離した瞬間を取得）
};
