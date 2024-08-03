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
	//キーボード
	unique_ptr<Keyboard> _key;	
	//キー状態
	Keyboard::State _keyState;						
	//押されているボタン
	Keyboard::KeyboardStateTracker _keyTracker;			

	//マウス
	unique_ptr<Mouse> _mouse;							
	//マウス状態
	Mouse::State _mouseState;							
	//マウス状態（ボタンを押した・離した瞬間を取得）
	Mouse::ButtonStateTracker _mouseTracker;	

	//コンストラクタ
	InputManager();						
	InputManager(const InputManager&) = delete;
	//デストラクタ
	~InputManager();

public:
	//シングルトンを返す
	static InputManager& Instance();

	//初期化
	void Init();

	//入力情報を更新する関数
	void UpdateInput();									

	//キー状態を取得
	Keyboard::State KeyState();							

	//キー状態を取得（キーを押した・離した瞬間を取得）
	Keyboard::KeyboardStateTracker KeyTracker();		

	//マウス状態を取得
	Mouse::State MouseState();							

	//マウス状態を取得（ボタンを押した・離した瞬間を取得）
	Mouse::ButtonStateTracker MouseTracker();			
};
