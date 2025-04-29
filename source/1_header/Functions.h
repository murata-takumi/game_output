#pragma once
#include "Includes.h"

namespace fs = std::filesystem;

/// <summary>
/// �q�[�v�v���p�e�B��t�@�C���p�X�ǂݍ��ݗp�֐����܂Ƃ߂����O���
/// </summary>
namespace
{
	const unsigned char _constSize = 0xff;	//�萔�o�b�t�@�[�̃T�C�Y��256�̔{���ɒ������邽�߂̃f�[�^

	//�q�[�v�ݒ�p�ϐ�
	CD3DX12_HEAP_PROPERTIES _uploadHeapProp  = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_HEAP_PROPERTIES _defHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	CD3DX12_HEAP_PROPERTIES _writeHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	/// <summary>
	/// wstring�^�̕������string�^�ɕϊ�����֐�
	/// </summary>
	/// <param name="value">wstring�^�̕�����</param>
	/// <returns>string�^�̕�����</returns>
	string ToString(const wstring& value)
	{
		auto length = WideCharToMultiByte(CP_UTF8, 0U, value.data(), -1, nullptr, 0, nullptr, nullptr);	//�o�C�g�����擾
		auto buffer = new char[length];																	//char�^�̕�������쐬

		WideCharToMultiByte(CP_UTF8, 0u, value.data(), -1, buffer, length, nullptr, nullptr);			//������ɏ�������

		string result(buffer);																			//string�^�ɕ������ϊ�
		delete[] buffer;																				//��������폜
		buffer = nullptr;																				//��������J��

		return result;
	}

	/// <summary>
	/// string�^�̃e�N�X�`���̃t�@�C������wstring�^�ɕϊ�����֐�
	/// </summary>
	/// <param name="str">�e�N�X�`���̃t�@�C����</param>
	/// <returns>wstring�^�֕ϊ������t�@�C����</returns>
	wstring ToWideString(const string& str)
	{
		auto num1 = MultiByteToWideChar(				//�Ăяo��1��ځi�����񐔂𓾂�j
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1,
			nullptr,
			0);

		wstring wstr;									//�Ԃ�l�ƂȂ�wstring�^�̕������錾
		wstr.resize(num1);								//�����񐔂����킹��

		auto num2 = MultiByteToWideChar(				//�Ăяo��2��ځi�t�@�C�������R�s�[����j
			CP_ACP,
			MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
			str.c_str(),
			-1,
			&wstr[0],
			num1);

		assert(num1 == num2);							//�����񐔂���v���Ă��邩(�ꉞ)�m�F

		return wstr;
	}

	/// <summary>
	/// �t�@�C���̃p�X����f�B���N�g���̃p�X���擾����֐�
	/// </summary>
	/// <param name="origin">�t�@�C���̃p�X</param>
	/// <returns>�f�B���N�g���̃p�X</returns>
	wstring GetDirectoryPath(const wstring& origin)
	{
		fs::path p = origin.c_str();			//wstring�^����wchar_t*�^�ɃL���X�g
		return p.remove_filename().c_str();		//�t�@�C�������폜���ĕԂ�
	}

	/// <summary>
	///	�p�X����t�@�C���̃p�X�݂̂��擾����֐�
	/// </summary>
	/// <param name="origin">��΃p�X</param>
	/// <returns>�t�@�C���̃p�X</returns>
	wstring GetFilePath(const char* origin)
	{
		fs::path file = origin;
		return file.filename().c_str();	//�t�@�C�������擾���ĕԂ�
	}

	/// <summary>
	/// �e�N�X�`���̊g���q�𓾂�֐�(string�p)
	/// </summary>
	/// <param name="path">�e�N�X�`���̃t�@�C����</param>
	/// <returns>�g���q</returns>
	string FileExtension(const string& path)
	{
		size_t idx = path.rfind('.');						//�t�@�C��������u.�v�̃C���f�b�N�X���擾
		return path.substr(idx + 1, path.length() - 1);		//�u.�v�̎��̕�������t�@�C�����̖����܂ł̕�����(�g���q)��Ԃ�
	}


	/// <summary>
	/// �t�@�C���̊g���q���擾����(wstring�p)
	/// </summary>
	/// <param name="path">�t�@�C���̃p�X</param>
	/// <returns>�g���q</returns>
	wstring FileExtension(const wstring& path)
	{
		size_t idx = path.find(L'.');							//�g���q�̈ʒu���擾
		return path.substr(idx + 1, path.length() - idx - 1);	//�g���q���擾
	}

	/// <summary>
	/// �t�@�C���̊g���q������������
	/// </summary>
	/// <param name="origin">�t�@�C���̃p�X</param>
	/// <param name="ext">�㏑������g���q</param>
	/// <returns>�g���q�������������t�@�C���̃p�X</returns>
	wstring ReplaceExtension(const wstring& origin, const char* ext)
	{
		fs::path p = origin.c_str();
		return p.replace_extension(ext).c_str();
	}

	/// <summary>
	/// LARGE_INTEGER�^���m�̍������擾����֐�
	/// </summary>
	/// <param name="a">������1</param>
	/// <param name="b">������2</param>
	/// <returns>����</returns>
	double
	GetLIntDiff(LARGE_INTEGER a, LARGE_INTEGER b)
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);														//���g�����擾

		double ret =																			//LARGE_INTEGER�^�̍��������g���Ŋ������l
			static_cast<double>(a.QuadPart - b.QuadPart) / static_cast<double>(freq.QuadPart);

		return ret;
	}

	/// <summary>
	/// ���񏈗������s����֐�
	/// </summary>
	/// <param name="func">���񏈗������������_��</param>
	void
	ParallelProcess(const function<void(void)>& func)
	{
		thread th(func);	//�����_������������X���b�h���쐬
		th.detach();		//�X���b�h�̊Ǘ��������
	}
}