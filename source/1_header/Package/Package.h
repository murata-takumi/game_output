#pragma once
#include "Package/MemoryMappedFile.h"

const int PACKAGE_FORMAT_MAX = 32;					//�t�H�[�}�b�g���̍ő咷��

/// <summary>
/// �w�b�_�[
/// </summary>
struct PackageHeader
{
	wchar_t formatName[PACKAGE_FORMAT_MAX];
	UINT fileNum;
};

/// <summary>
/// �t�@�C���f�[�^
/// </summary>
struct FileData
{
	wchar_t fileName[MAX_PATH];	//�t�@�C����
	UINT size;						//�t�@�C���T�C�Y
	BYTE* ptr;						//�t�@�C���̐擪�|�C���^
	BYTE tail;						//�t�@�C���̖���
};

/// <summary>
/// �t�@�C���̃p�b�P�[�W���A�ǂݍ��݂���������N���X
/// </summary>
class Package
{
private:
	wchar_t _formatName[PACKAGE_FORMAT_MAX];						//�t�H�[�}�b�g��
	list<basic_string<wchar_t>> _fileList;							//�t�@�C�����̃��X�g
	map<basic_string<wchar_t>, FileData> _fileDataMap;				//�t�@�C�����ƃt�@�C���f�[�^�̘A�z�z��
	MemoryMappedFile _packageFile;									//�p�b�P�[�W�t�@�C��

	void EnumFiles(const wchar_t* dir, const wchar_t* wildCard);	//�t�@�C����񋓂���֐�
public:
	Package();																	//�R���X�g���N�^
	~Package();																	//�f�X�g���N�^

	bool SetFormatName(wchar_t formatname[PACKAGE_FORMAT_MAX]);					//�t�H�[�}�b�g����o�^����֐�
	bool CreatePackage(const wchar_t* dirName, const wchar_t* outFileName);		//�p�b�P�[�W���쐬����֐�
	bool OpenPackage(const wchar_t* fileName);									//�p�b�P�[�W���J���֐�
	bool LoadPackage(BYTE* ptr);												//�p�b�P�[�W���̃f�[�^��ǂݍ��ފ֐�

	bool GetFileData(const wchar_t* fileName, FileData* pOutData);				//�t�@�C���̃f�[�^��ǂݍ��ފ֐�
	
	int GetFileNum();															//�t�@�C�������擾����֐�

	bool CreateDirectoryAndFile(const basic_string<wchar_t> fileName);			//�f�B���N�g���Ƃ��̒��̃t�@�C�����쐬����֐�
	bool RemoveFileInDiretory(const basic_string<wchar_t> dirName);				//�f�B���N�g�����̃t�@�C�������ׂč폜����֐�
};