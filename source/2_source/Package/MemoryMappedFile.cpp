#include "Package/MemoryMappedFile.h"

/// <summary>
/// �R���X�g���N�^
/// �e�ϐ�������������
/// </summary>
MemoryMappedFile::MemoryMappedFile()
	:_hFile(NULL), _hMap(NULL), _pPointer(nullptr), _fileSize(0)
{

}

/// <summary>
/// �f�X�g���N�^
/// ���̃C���X�^���X���J������
/// </summary>
MemoryMappedFile::~MemoryMappedFile()
{
	this->Release();
}

/// <summary>
/// �f�[�^��S�ĉ���E����������֐�
/// </summary>
void
MemoryMappedFile::Release()
{
	if (_pPointer != nullptr)
	{
		UnmapViewOfFile(_pPointer);
		_pPointer = nullptr;
	}
	if (_hMap != NULL)
	{
		CloseHandle(_hMap);
		_hMap = NULL;
	}
	if (_hFile != NULL)
	{
		CloseHandle(_hFile);
		_hFile = NULL;
	}
	_fileSize = 0;
}

/// <summary>
/// �t�@�C�����J���֐�
/// </summary>
/// <param name="fileName">�t�@�C����</param>
/// <param name="readWriteFlag">�ǂݏ����t���O</param>
/// <param name="openFlag">�t�@�C���I�[�v���t���O</param>
/// <returns>�����������������ǂ���</returns>
bool
MemoryMappedFile::Open(
	const wchar_t* fileName,DWORD readWriteFlag, DWORD openFlag
)
{
	_hFile = CreateFile(fileName, readWriteFlag, 0, 0, openFlag,		//�t�@�C�����J��
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (_hFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD mapFlag = PAGE_READWRITE;										//�t�@�C���}�b�s���O�I�u�W�F�N�g���쐬
	if (readWriteFlag == GENERIC_READ)
	{
		mapFlag = PAGE_READONLY;
	}
	_hMap = CreateFileMapping(_hFile, 0, mapFlag, 0, 0, fileName);
	if (_hMap == INVALID_HANDLE_VALUE)
	{
		CloseHandle(_hFile);
		_hFile = NULL;
		return false;
	}

	DWORD mapViewFlag = FILE_MAP_WRITE;									//�|�C���^���擾
	if (mapFlag == PAGE_READONLY)
	{
		mapViewFlag = FILE_MAP_READ;
	}
	_pPointer = MapViewOfFile(_hMap, mapViewFlag, 0, 0, 0);
	if (_pPointer == nullptr)
	{
		CloseHandle(_hMap);
		CloseHandle(_hFile);
		_hMap = NULL;
		_hFile = NULL;
		return false;
	}

	DWORD fileSize;														//�t�@�C���T�C�Y���擾
	_fileSize = ::GetFileSize(_hFile, &fileSize);

	return true;
}

/// <summary>
/// �t�@�C���f�[�^�̐擪�|�C���^��Ԃ��֐�
/// </summary>
/// <returns>�|�C���^</returns>
void*
MemoryMappedFile::GetPtr()
{
	return _pPointer;
}

/// <summary>
/// �t�@�C���T�C�Y��Ԃ��֐�
/// </summary>
/// <returns>�t�@�C���T�C�Y</returns>
DWORD
MemoryMappedFile::GetSize()
{
	return _fileSize;
}