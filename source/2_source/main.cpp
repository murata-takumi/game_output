#include"Application.h"

#include "Package/Package.h"

/// <summary>
/// プログラム実行時最初に呼び出される関数
/// </summary>
/// <returns>関数が正常に終了したかどうか</returns>
#ifdef _DEBUG
int main() {
#else
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
#endif

	// Applicationインスタンス取得
	auto& app = Application::Instance();	

	// Applicationインスタンスの初期化が成功しなければ異常終了
	if (!app.Init())						
	{
		return -1;
	}

	// ゲーム画面を描画
	app.Run();						
	// 終了時の後始末
	app.Terminate();						

	// 正常終了
	return 0;								
}