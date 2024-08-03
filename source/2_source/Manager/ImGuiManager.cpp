#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

//フォントを格納しているパス
const char* FONT_PATH = "Asset/font/BIZUDPGothic-Bold.ttf";

/// <summary>
/// コンストラクタ
/// ImGui関連の初期化を行う
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="hwnd">ウィンドウハンドル</param>
ImGuiManager::ImGuiManager():
	_windowWidth(static_cast<float>(Application::Instance().GetWindowSize().cx)),
	_windowHeight(static_cast<float>(Application::Instance().GetWindowSize().cy)),
	_size(18.0f),_speed(1.0f),_animSliderValue(0.0f), _fps(0.0f)
{

}

/// <summary>
/// デストラクタ
/// </summary>
ImGuiManager::~ImGuiManager()
{

}

/// <summary>
/// 初期化関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
void
ImGuiManager::Init(HWND hwnd)
{
	_fpsFlag |= ImGuiWindowFlags_NoMove;								//FPSを表示するウィンドウが動かないよう設定
	_fpsFlag |= ImGuiWindowFlags_NoTitleBar;							//タイトルバーを表示しない
	_fpsFlag |= ImGuiWindowFlags_NoResize;								//ウィンドウサイズを変えない
	_fpsFlag |= ImGuiWindowFlags_NoScrollbar;							//スクロールバーを表示しない

	_animFlag |= ImGuiWindowFlags_NoMove;								//アニメーション表示ウィンドウも同様に設定
	_animFlag |= ImGuiWindowFlags_NoResize;								//ウィンドウサイズを変えない

	if (ImGui::CreateContext() == nullptr)								//imguiの初期化
	{
		assert(0);
		return;
	}
	bool bInResult = ImGui_ImplWin32_Init(hwnd);
	if (!bInResult)
	{
		assert(0);
		return;
	}

	Dx12Wrapper::Instance().CreateDescriptorHeap(_heapForImgui,							//ImGui用ヒープを作成
		D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 0, 1,
		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);


	bInResult = ImGui_ImplDX12_Init(
		Dx12Wrapper::Instance().Device(),
		3,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		_heapForImgui.Get(),
		_heapForImgui->GetCPUDescriptorHandleForHeapStart(),
		_heapForImgui->GetGPUDescriptorHandleForHeapStart());
	if (!bInResult)
	{
		assert(0);
		return;
	}

	ImGuiIO& io = ImGui::GetIO();										//フォントを設定
	io.Fonts->AddFontFromFileTTF(
		FONT_PATH,
		_size, NULL, io.Fonts->GetGlyphRangesJapanese());
}

/// <summary>
/// シングルトンを返す関数
/// </summary>
/// <returns>シングルトン</returns>
ImGuiManager&
ImGuiManager::Instance()
{
	static ImGuiManager instance;
	return instance;
}

bool
ImGuiManager::CanActorTrans()const
{
	return _canActorControll;
}

bool
ImGuiManager::IsWriteGrid()const
{
	return _isWriteGrid;
}

/// <summary>
/// 各種ウィンドウの描画を実装する関数
/// </summary>
void
ImGuiManager::ImGuiDraw()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//FPSウィンドウの表示処理
	{
		ImGui::Begin("FPS", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(DIFF,DIFF));												//ウィンドウ位置を決定
		ImGui::SetWindowSize(ImVec2(_size * 11, _size * 3));								//ウィンドウサイズを設定

		if (_fps <= 30.0f)ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));	//FPSが30以下の場合文字の色を赤くする
		ImGui::Text("FPS:%.1f", _fps);														//FPSを表示
		if (_fps <= 30.0f)ImGui::PopStyleColor();

		ImGui::Text(Dx12Wrapper::Instance().Perspective() ? "Perspective" : "OrthoGraphic");					//透視投影か平行投影か表示

		ImGui::End();
	}

	//アクターが移動する際のウィンドウ
	{
		ImGui::Begin("Translation", 0, _fpsFlag);

		ImGui::SetWindowPos(ImVec2(_windowWidth - (DIFF + _size * 18), DIFF));
		ImGui::SetWindowSize(ImVec2(_size * 18,_size * 36));

		ImGui::Checkbox("WriteGrid", &_isWriteGrid);										//グリッドを描画するか決める

		if (ImGui::Checkbox("CanActorControll", &_canActorControll))						//アクターが移動可能かどうか決める
		{
			if (_canActorControll)															//チェックボックスをtrueにした時
			{
				_actor->StartControll();

				_canCallAnim = false;
			}
			else																			//チェックボックスをfalseにした時
			{
				_actor->EndControll();														//アクターの移動・回転を初期化
				Dx12Wrapper::Instance().ResetCoordinates(0.0f,0.0f);											//カメラを初期化
			}
		}

		ImGui::Checkbox("IsOnGround", &_actor->_isOnGround);

		ImGui::End();
	}

	//アニメーション名を並べたウィンドウの表示処理
	{
		ImGui::Begin("Animation",0,_animFlag);

		ImGui::SetWindowPos(ImVec2(DIFF, _windowHeight - (ANIMATION_HEIGHT + DIFF)));		//ウィンドウ座標を設定
		ImGui::SetWindowSize(ImVec2(_windowWidth - (DIFF * 2),ANIMATION_HEIGHT));			//ウィンドウサイズを設定
		ImGui::BeginDisabled(_canActorControll);											//アクターが移動可能なときはこのウィンドウを非活性化

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
		if (ImGui::BeginCombo("##", _currentImGuiAnim.c_str()))
		{
			for (int i = 0; i < _animStr.size(); ++i)
			{
				_comboSelect = (_currentImGuiAnim.c_str() == _animStr[i].c_str());
				if (ImGui::Selectable(_animStr[i].c_str(), _comboSelect))
				{
					_currentImGuiAnim = _animStr[i].c_str();

					_maxDur = _actor->GetAnimDuration(_currentImGuiAnim) - _actor->GetSecondFrame();
					_tick = _actor->GetAnimTickPerSpeed(_currentImGuiAnim);
					_animSliderValue = 0.0f;

					_actor->InitPose();
					_actor->SetAnimationTime(_animSliderValue);										//スライダーの値をアクターに渡す

					_canCallAnim = false;
				}

				if (_comboSelect)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if(ImGui::Button("Resume"))
		{
			QueryPerformanceCounter(&_beforeFrameTime);										//アニメーション開始時のフレームの時間を取得

			_animSliderValue = 0.0f;

			_actor->SetAnimStr(_currentImGuiAnim);											//実行するアニメーションを指定
			_actor->SetAnimationTime(_animSliderValue);										//実行するアニメーションを指定

			_canCallAnim = true;
		}

		ImGui::SameLine();

		if (ImGui::Button("Reset"))
		{
			_animSliderValue = 0.0f;

			_actor->InitPose();																//アクターのポーズをリセットする
			_actor->SetAnimationTime(_animSliderValue);										//スライダーの値をアクターに渡す

			_canCallAnim = false;
		}

		ImGui::SliderFloat("###", &_speed, 0.1f, 2.0f);										//アニメーションの再生速度
		
		ImGui::SameLine();

		if (ImGui::Button("1.0"))															//再生速度を1に直す
		{
			_speed = 1.0f;
		}
		ImGui::PopItemWidth();

		_animSliderValue = clamp(_animSliderValue,0.0f, _maxDur);
		if (ImGui::SliderFloat("Animation", &_animSliderValue, 0.0f, _maxDur))
		{
			if (_canCallAnim)_canCallAnim = false;									//アニメーション実行中だったら止める
			_actor->SetAnimationTime(_animSliderValue);								//スライダーの値をアクターに渡す
		}

		ImGui::EndDisabled();
		ImGui::End();
	}

	Update();																		//描画以外の処理

	ImGui::Render();

	ID3D12DescriptorHeap* heap[] = { _heapForImgui.Get()};							//ImGui用ヒープをセット
	Dx12Wrapper::Instance().CommandList()->SetDescriptorHeaps(1, heap);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Dx12Wrapper::Instance().CommandList());
}

/// <summary>
/// シーンから受け取ったデータを反映する関数
/// </summary>
/// <param name="data"FPS</param>
void
ImGuiManager::SetFPS(float fps)
{
	_fps = fps;
}

/// <summary>
/// アクターと地面の座標ごとの真理値の当たり判定を受け取る関数
/// </summary>
/// <param name="intersects">真理値ベクトル</param>
void
ImGuiManager::SetActorColBool(bool intersects[])
{
	for(int i = 0; i < 6; i++)
	{
		_intersects[i] = intersects[i];
	}
}

/// <summary>
/// アクターを受け取る関数
/// </summary>
/// <param name="actor">アクターのベクトル</param>
void
ImGuiManager::SetActor(vector<shared_ptr<FBXBase>> actorAndObjects)
{
	_ground = actorAndObjects[1];

	//ベクトルの中からアクターを探して取得
	for (const auto& object : actorAndObjects)						
	{
		if (_actor = dynamic_pointer_cast<FBXActor>(object))
		{
			break;
		}
	}

	//アクターのアニメーション名をこちら側に格納する
	for (auto str : _actor->GetAnimstr())							
	{
		_animStr.push_back(str);
	}

	//初期アニメーションを設定
	_currentImGuiAnim = _animStr[0];								

	//アニメーションの総時間を取得
	_maxDur = _actor->GetAnimDuration(_currentImGuiAnim)			
				- _actor->GetSecondFrame();
	//アニメーションの処理回数を取得
	_tick = _actor->GetAnimTickPerSpeed(_currentImGuiAnim);			
	//2個目のフレームを取得
	_animSliderValue = 0.0f;										
}

/// <summary>
/// アニメーション名の配列を初期化する関数
/// </summary>
void
ImGuiManager::ResetAnimStr()
{
	_animStr.clear();
}

/// <summary>
/// 描画以外の処理を実行する関数
/// </summary>
void
ImGuiManager::Update()
{
	if (_canCallAnim)
	{
		QueryPerformanceCounter(&_crrntFrameTime);								//現フレームの時間を取得

		auto diff = GetLIntDiff(_crrntFrameTime, _beforeFrameTime);				//現フレームと前フレームの時間の差分をfloatに変換

		_animSliderValue +=														//差分を加算
			static_cast<float>(diff) * _speed * _tick;

		_beforeFrameTime = _crrntFrameTime;										//前フレームの時間を更新

		_actor->SetAnimationTime(_animSliderValue);								//スライダーの値をアクターに渡す
	}
}