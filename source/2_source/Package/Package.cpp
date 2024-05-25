#include "Package/Package.h"

#include <fstream>
#include <filesystem>
#include <fileapi.h>

/// <summary>
/// �������u������֐�
/// </summary>
/// <param name="source">�u���Ώۂ̕�����</param>
/// <param name="pattern">�u���Ώۂ̃p�^�[��</param>
/// <param name="placement">�u�����镶����</param>
/// <returns>�u����̕�����</returns>
basic_string<wchar_t> _treplace(
	const basic_string<wchar_t>& source,
	const basic_string<wchar_t>& pattern,
	const basic_string<wchar_t>& placement
)
{
	basic_string<wchar_t> result;													//�Ԃ�l
	basic_string<wchar_t>::size_type pos_before = 0;								//�����ʒu
	basic_string<wchar_t>::size_type pos = 0;										//������̏����ʒu
	basic_string<wchar_t>::size_type len = pattern.size();							//�p�^�[���̒���

	while ((pos = source.find(pattern, pos)) != basic_string<wchar_t>::npos){		//��������ɒu���Ώۂ����݂���ꍇ
		result.append(source, pos_before, pos - pos_before);						//�Ԃ�l�ɒu���Ώێ�O�܂ł̕������ǉ�
		result.append(placement);													//�����ɒu���������ǉ�
		pos += len;																	//�����ʒu���X�V
		pos_before = pos;															//�����ʒu���X�V
	}

	result.append(source, pos_before, source.size() - pos_before);					//�u���Ώۈȍ~�̕������ǉ�
	return result;
}

/// <summary>
/// �t�@�C����񋓂���֐�
/// </summary>
/// <param name="dir">�f�B���N�g���p�X</param>
/// <param name="wildCard">���C���h�J�[�h</param>
void
Package::EnumFiles(const wchar_t* dir, const wchar_t* wildCard)
{
	basic_string<wchar_t> currentDir = dir;
	basic_string<wchar_t> relatedDir = dir;
	currentDir += _T("/");
	currentDir += wildCard;
	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(currentDir.c_str(), &fd);										//�f�B���N�g��������
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return;
	}

	do {																						//�f�B���N�g�����̑S�f�[�^�ɑ΂����s

		if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)									//�f�[�^���f�B���N�g���������ꍇ			
			{
				basic_string<wchar_t> childDir = dir;											//�q�f�B���N�g���ɑ΂��ċA����
				childDir += _T("/");
				childDir += fd.cFileName;
				this->EnumFiles(childDir.c_str(), wildCard);
			}
			else																				//�f�[�^���t�@�C���������ꍇ
			{
				basic_string<wchar_t> tmp = relatedDir;											//�A�z�z��ɒǉ�
				tmp += _T("/");
				tmp += fd.cFileName;

				basic_string<wchar_t> correct = _treplace(tmp,_T("\\"),_T("/"));
				_fileList.push_back(correct.c_str());
			}
		}

	} while (FindNextFile(hFind, &fd) == TRUE);
}

/// <summary>
/// �R���X�g���N�^
/// �t�H�[�}�b�g��������������
/// </summary>
Package::Package()
{
	memset(_formatName, 0, sizeof(_formatName));
}

/// <summary>
/// �f�X�g���N�^
/// </summary>
Package::~Package()
{

}

/// <summary>
/// �t�H�[�}�b�g����o�^����֐�
/// </summary>
/// <param name="formatname">�o�^�������t�H�[�}�b�g��</param>
/// <returns>�����������������ǂ���</returns>
bool 
Package::SetFormatName(wchar_t formatname[PACKAGE_FORMAT_MAX])
{
	wcscpy_s(_formatName, formatname);
	return true;
}

/// <summary>
/// �w�肵���f�B���N�g���̑S�t�@�C�����p�b�P�[�W������֐�
/// </summary>
/// <param name="dirName">�f�B���N�g����</param>
/// <param name="outFileName">�o�̓t�@�C����</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::CreatePackage(const wchar_t* dirName, const wchar_t* outFileName)
{
	basic_string<wchar_t> dir = dirName;												//�f�B���N�g�������R�s�[

	this->EnumFiles(dir.c_str(), _T("*.*"));											//�t�@�C����

	HANDLE hOutFile = CreateFile(outFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,		//�n���h���擾
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	DWORD writeByte = 0;																//�������݃o�C�g��

	PackageHeader header = {};															//�w�b�_�[�ւ̏�������
	wcscpy_s(header.formatName, _formatName);											//�t�H�[�}�b�g��
	header.fileNum = (UINT)_fileList.size();											//�t�@�C����
	WriteFile(hOutFile, &header, sizeof(header), &writeByte, NULL);						//�w�b�_�[��������

	list<basic_string<wchar_t>>::iterator it;											//�t�@�C���f�[�^��������
	for (it = _fileList.begin(); it != _fileList.end(); it++)							//�f�B���N�g�����̑S�t�@�C���ɑ΂��������s��
	{
		MemoryMappedFile file;															//�t�@�C����������
		if (!file.Open(it->c_str(), GENERIC_READ))
		{
			CloseHandle(hOutFile);
			return false;
		}

		FileData data = {};																//�t�@�C���f�[�^��������
		wcscpy_s(data.fileName, it->c_str());
		data.size = file.GetSize();
		data.tail = '\0';

		WriteFile(hOutFile, data.fileName, sizeof(data.fileName), &writeByte, NULL);	//�t�@�C������������
		WriteFile(hOutFile, &data.size, sizeof(data.size), &writeByte, NULL);			//�t�@�C���T�C�Y��������
		WriteFile(hOutFile, file.GetPtr(), file.GetSize(), &writeByte, NULL);			//�t�@�C���f�[�^��������
		WriteFile(hOutFile, &data.tail, sizeof(BYTE), &writeByte, NULL);				//�f�[�^������������
	}

	CloseHandle(hOutFile);																//�t�@�C���n���h�����N���[�Y

	return true;
}

/// <summary>
/// �p�b�P�[�W�t�@�C�����J���֐�
/// </summary>
/// <param name="fileName">�t�@�C����</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::OpenPackage(const wchar_t* fileName)
{
	if(!_packageFile.Open(fileName))								//�p�b�P�[�W�t�@�C�����J��
	{
		_packageFile.Release();
		return false;
	}

	return this->LoadPackage((BYTE*)_packageFile.GetPtr());			//�p�b�P�[�W�t�@�C���̃|�C���^���擾�ł������ǂ���
}

/// <summary>
/// �p�b�P�[�W�f�[�^��ǂݎ��֐�
/// </summary>
/// <param name="ptr">�p�b�P�[�W�f�[�^�̐擪�|�C���^</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::LoadPackage(BYTE* ptr)
{
	BYTE* tempPtr = ptr;													//�w�b�_�[��ǂݍ��݁A���̃T�C�Y���|�C���^���Y����
	PackageHeader header = {};
	memcpy(&header, tempPtr, sizeof(header));	
	tempPtr += sizeof(header);

	if (_tcscmp(header.formatName, _formatName) != 0)						//�p�b�P�[�W�̃t�H�[�}�b�g���̃`�F�b�N
	{
		_packageFile.Release();
		return false;
	}

	for (UINT i = 0; i < header.fileNum; ++i)								//�w�b�_�[�ɓo�^����Ă���S�t�@�C���ɑ΂����s
	{
		FileData data = {};
		memcpy(data.fileName, tempPtr, sizeof(data.fileName));				//�t�@�C�����ǂݍ���
		tempPtr += sizeof(data.fileName);
		memcpy(&data.size, tempPtr, sizeof(data.size));						//�t�@�C���T�C�Y�ǂݍ���
		tempPtr += sizeof(data.size);
		data.ptr = tempPtr;													//�t�@�C���̐擪�|�C���^�擾
		tempPtr += data.size;
		memcpy(&data.tail, tempPtr, sizeof(data.tail));						//�f�[�^�����ǂݍ���
		tempPtr += sizeof(data.tail);

		_fileDataMap.insert(												//�f�[�^��}��
			pair<basic_string<wchar_t>, FileData>(data.fileName, data)
		);
	}

	return true;
}

/// <summary>
/// �w�肵���t�@�C���f�[�^���擾����֐�
/// </summary>
/// <param name="fileName">�w�肷��t�@�C����</param>
/// <param name="pOutData">�t�@�C���f�[�^</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::GetFileData(const wchar_t* fileName, FileData* pOutData)
{
	if (_fileDataMap.find(fileName) != _fileDataMap.end())				//�t�@�C���������Ɍ������A�Ή�����f�[�^���R�s�[����
	{
		wcscpy_s(pOutData->fileName, _fileDataMap[fileName].fileName);
		pOutData->ptr = _fileDataMap[fileName].ptr;
		pOutData->size = _fileDataMap[fileName].size;
		pOutData->tail = _fileDataMap[fileName].tail;

		return true;
	}

	return false;
}

/// <summary>
/// �p�b�P�[�W�Ɋi�[����Ă���t�@�C������Ԃ��֐�
/// </summary>
/// <returns></returns>
int
Package::GetFileNum()
{
	return (int)_fileDataMap.size();
}

/// <summary>
/// .pak�t�@�C������Ƀf�B���N�g���Ƀf�[�^�t�@�C�����쐬����֐�
/// </summary>
/// <param name="fileName">.pak�t�@�C����</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::CreateDirectoryAndFile(const basic_string<wchar_t> fileName)
{
	OpenPackage(fileName.c_str());

	basic_string<wchar_t> dirName;										//.pak�t�@�C���̊g���q���폜���A�f�B���N�g���p�X�ɂ���
	dirName.append(fileName,0,fileName.size() - 4);

	map<basic_string<wchar_t>, FileData>::iterator it;
	for (it = _fileDataMap.begin(); it != _fileDataMap.end(); it++)		//.pak���Ɋi�[����Ă����f�[�^��S�ăt�@�C���ɏ����o��
	{
		FileData fd;
		GetFileData(it->first.c_str(), &fd);
		void* data = fd.ptr;
		size_t size = fd.size;

		ofstream ofs(it->first.c_str(),
			std::ios::out | std::ios::binary);
		if (!ofs)
		{
			assert(0);
		}
		ofs.write(static_cast<const char*>(data), size);
		ofs.close();
	}

	return true;
}

/// <summary>
/// �f�B���N�g�����̃t�@�C�������ׂč폜����֐�
/// </summary>
/// <param name="dirName">�f�B���N�g���p�X</param>
/// <returns>�����������������ǂ���</returns>
bool
Package::RemoveFileInDiretory(const basic_string<wchar_t> dirName)
{
	basic_string<wchar_t> currentDir = dirName;
	basic_string<wchar_t> wildCard = _T("*.*");

	currentDir += _T("/");
	currentDir += wildCard;
	WIN32_FIND_DATA fd = {};
	HANDLE hFind = FindFirstFile(currentDir.c_str(), &fd);										//�f�B���N�g�������݂��邩�`�F�b�N
	if (hFind == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	do {																						//�f�B���N�g�������S�f�[�^�ɑ΂����s

		if (_tcscmp(fd.cFileName, _T(".")) != 0 && _tcscmp(fd.cFileName, _T("..")) != 0)
		{
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)									//�f�B���N�g���������ꍇ
			{
				basic_string<wchar_t> childDir = dirName;										//�q�f�B���N�g���ɑ΂��ċA�Ăяo��
				childDir += _T("/");
				childDir += fd.cFileName;
				this->RemoveFileInDiretory(childDir);
			}
			else																				//�t�@�C���������ꍇ
			{
				basic_string<wchar_t> fileName = dirName + _T("/") + fd.cFileName;				//�t�@�C�������擾���A�폜
				filesystem::remove(fileName.c_str());
			}
		}

	} while (FindNextFile(hFind, &fd) == TRUE);

	return true;
}