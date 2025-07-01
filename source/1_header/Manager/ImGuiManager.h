#pragma once
#include "Includes.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

class Dx12Wrapper;
class IFBX;
class FBXObject;
class FBXActor;
class Vector3;
class ImGuiManager
{
public:
	string str;
	//初期化関数
	void Init(HWND hwnd);

	//シングルトンを返す関数
	static ImGuiManager& Instance();

	//アクターが移動可能かどうかを返す関数
	bool CanActorTrans()const;										
	//グリッドを描画するかどうかを返す関数
	bool IsWriteGrid()const;										

	//各ウィンドウの描画処理
	void ImGuiDraw();												

	//PlaySceneから受け取ったデータを反映する関数
	void SetFPS(float fps);
	//アクターを受け取る関数
	void SetActor(const map<string, shared_ptr<IFBX>> actorAndObjs);
	//アニメーション名の配列を初期化する関数
	void ResetAnimStr();

	//文字列を追加する関数
	void AddText(int idx, const char* text);

	//float型の数値を追加する関数
	void AddLabelAndFloat(const char* label,float value);

	//bool型の値を追加する関数
	void AddLabelAndBool(const char* label, bool value);

	bool _isCrossed;
private:
	//コンストラクタ
	ImGuiManager();
	ImGuiManager(const ImGuiManager&) = delete;
	//デストラクタ
	~ImGuiManager();

	//アクター
	shared_ptr<IFBX> _actor;				
	//地面
	shared_ptr<IFBX> _ground;				

	//文字列
	map<int,string> _texts;

	//表示ラベルとfloat値
	map<const char*, float> _labelAndFloats;

	//表示ラベルとbool値
	map<const char*, bool> _labelAndBools;

	ComPtr<ID3D12DescriptorHeap> _heapForImgui;		//ImGui用ヒープ

	string _currentImGuiAnim;						//アクターが現在実行しているアニメーション名
	vector<string> _animStr;						//全アニメーション名

	ImGuiWindowFlags _fpsFlag = 0;					//FPS表示用ウィンドウの設定フラグ
	ImGuiWindowFlags _animFlag = 0;					//アニメーション表示用ウィンドウの設定フラグ

	LARGE_INTEGER _beforeFrameTime;					//前フレームの時間
	LARGE_INTEGER _crrntFrameTime;					//現フレームの時間

	//アクターと地面の座標ごとの当たり判定を表示する真理値
	bool _intersects[6];

	bool _canActorControll = false;					//操作可能かどうかを決める真理値
	bool _comboSelect = false;						//アニメーションのコンボボックスの内どれが選択されているかを決める真理値
	bool _canCallAnim = false;						//アニメーションを実行するかどうかを決める真理値
	bool _isWriteGrid = true;						//グリッドを描画するかどうか決める真理値

	float _windowWidth;								//ウィンドウ幅
	float _windowHeight;							//ウィンドウ高さ

	float _size;									//フォントサイズ
	float _speed;									//アニメーションの再生スピード
	float _tick;									//

	float _maxDur;									//アニメーションの総時間
	float _animSliderValue;							//アニメーションの現在時間

	float _fps;										//1秒当たりのフレームレート

	void Update();									//描画以外の処理を実行する関数
};