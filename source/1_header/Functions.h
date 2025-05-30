#pragma once
#include "Includes.h"

namespace fs = std::filesystem;

/// <summary>
/// ヒーププロパティやファイルパス読み込み用関数をまとめた名前空間
/// </summary>
namespace
{
	const unsigned char _constSize = 0xff;	//定数バッファーのサイズを256の倍数に調整するためのデータ

	//ヒープ設定用変数
	CD3DX12_HEAP_PROPERTIES _uploadHeapProp  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_HEAP_PROPERTIES _defHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES _writeHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	/// <summary>
	/// wstring型の文字列をstring型に変換する関数
	/// </summary>
	/// <param name="value">wstring型の文字列</param>
	/// <returns>string型の文字列</returns>
	string ToString(const wstring& value)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);	//バイト長を取得
		auto buffer = new char[length];																	//char型の文字列を作成

		WideCharToMultiByte(CP_UTF8, 0u, value.data(), -1, buffer, length, nullptr, nullptr);			//文字列に書き込み

		string result(buffer);																			//string型に文字列を変換
		delete[] buffer;																				//文字列を削除
		buffer = nullptr;																				//文字列を開放

		return result;
	}

	/// <summary>
	/// string型のテクスチャのファイル名をwstring型に変換する関数
	/// </summary>
	/// <param name="str">テクスチャのファイル名</param>
	/// <returns>wstring型へ変換したファイル名</returns>
	wstring ToWideString(const string& str)
	{
		auto num1 = MultiByteToWideChar(				//呼び出し1回目（文字列数を得る）
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1,
			nullptr,
			0);

		wstring wstr;									//返り値となるwstring型の文字列を宣言
		wstr.resize(num1);								//文字列数を合わせる

		auto num2 = MultiByteToWideChar(				//呼び出し2回目（ファイル名をコピーする）
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1,
			&wstr[0],
			num1);

		assert(num1 == num2);							//文字列数が一致しているか(一応)確認

		return wstr;
	}

	/// <summary>
	/// ファイルのパスからディレクトリのパスを取得する関数
	/// </summary>
	/// <param name="origin">ファイルのパス</param>
	/// <returns>ディレクトリのパス</returns>
	wstring GetDirectoryPath(const wstring& origin)
	{
		fs::path p = origin.c_str();			//wstring型からwchar_t*型にキャスト
		return p.remove_filename().c_str();		//ファイル名を削除して返す
	}

	/// <summary>
	///	パスからファイルのパスのみを取得する関数
	/// </summary>
	/// <param name="origin">絶対パス</param>
	/// <returns>ファイルのパス</returns>
	wstring GetFilePath(const char* origin)
	{
		fs::path file = origin;
		return file.filename().c_str();	//ファイル名を取得して返す
	}

	/// <summary>
	/// テクスチャの拡張子を得る関数(string用)
	/// </summary>
	/// <param name="path">テクスチャのファイル名</param>
	/// <returns>拡張子</returns>
	string FileExtension(const string& path)
	{
		size_t idx = path.rfind('.');						//ファイル名から「.」のインデックスを取得
		return path.substr(idx + 1, path.length() - 1);		//「.」の次の文字からファイル名の末尾までの文字列(拡張子)を返す
	}


	/// <summary>
	/// ファイルの拡張子を取得する(wstring用)
	/// </summary>
	/// <param name="path">ファイルのパス</param>
	/// <returns>拡張子</returns>
	wstring FileExtension(const wstring& path)
	{
		size_t idx = path.find(L'.');							//拡張子の位置を取得
		return path.substr(idx + 1, path.length() - idx - 1);	//拡張子を取得
	}

	/// <summary>
	/// ファイルの拡張子を書き換える
	/// </summary>
	/// <param name="origin">ファイルのパス</param>
	/// <param name="ext">上書きする拡張子</param>
	/// <returns>拡張子を書き換えたファイルのパス</returns>
	wstring ReplaceExtension(const wstring& origin, const char* ext)
	{
		fs::path p = origin.c_str();
		return p.replace_extension(ext).c_str();
	}

	/// <summary>
	/// LARGE_INTEGER型同士の差分を取得する関数
	/// </summary>
	/// <param name="a">数その1</param>
	/// <param name="b">数その2</param>
	/// <returns>差分</returns>
	double
	GetLIntDiff(LARGE_INTEGER a, LARGE_INTEGER b)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);														//周波数を取得

		double ret =																			//LARGE_INTEGER型の差分を周波数で割った値
			static_cast<double>(a.QuadPart - b.QuadPart) / static_cast<double>(freq.QuadPart);

		return ret;
	}

	/// <summary>
	/// 並列処理を実行する関数
	/// </summary>
	/// <param name="func">並列処理したいラムダ式</param>
	void
	ParallelProcess(const function<void(void)>& func)
	{
		thread th(func);	//ラムダ式を処理するスレッドを作成
		th.detach();		//スレッドの管理を手放す
	}
}