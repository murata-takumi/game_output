#pragma once
#include "Application.h"

/// <summary>
/// �p�b�P�[�W������t�@�C���̃f�[�^��ۑ�����N���X
/// </summary>
class MemoryMappedFile
{
protected:
	HANDLE _hFile;		//�t�@�C���̃n���h��
	HANDLE _hMap;		//�������}�b�v�̃n���h��
	void* _pPointer;	//�t�@�C���f�[�^�̐擪�|�C���^
	DWORD _fileSize;	//�t�@�C���T�C�Y

public:
	MemoryMappedFile();											//�R���X�g���N�^
	~MemoryMappedFile();										//�f�X�g���N�^

	void Release();												//�f�[�^��S�ĉ���E����������֐�

	bool Open(													//�t�@�C�����J���֐�
		const wchar_t* fileName,
		DWORD readWriteFlag = GENERIC_READ | GENERIC_WRITE,
		DWORD openFlag = OPEN_EXISTING
	);

	void* GetPtr();												//�t�@�C���f�[�^�̐擪�|�C���^��Ԃ��֐�

	DWORD GetSize();											//�t�@�C���T�C�Y��Ԃ��֐�
};