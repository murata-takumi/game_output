#pragma once
#include "Application.h"

#include <chrono>

/// <summary>
/// 各シーンオブジェクトのインターフェイス
/// </summary>
class Dx12Wrapper;
class FBXActor;
class SpriteManager;
class InputManager;
class SoundManager;
class ImGuiManager;
class IScene
{
	friend ImGuiManager;

public:

	//コンストラクタ
	IScene() = default;
	//デストラクタ
	~IScene() = default;

	//更新処理
	virtual void Update() = 0;				
	//シーン開始時の処理
	virtual void SceneStart() = 0;		
	//シーン終了時の処理
	virtual void SceneEnd() = 0;
protected:

	//モデルを描画するためのクラス
	virtual void ModelDraw() = 0;

	//エフェクト・UIを描画する関数
	virtual void EffectAndUIDraw() = 0;
};