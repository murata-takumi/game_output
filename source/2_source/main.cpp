#include"Application.h"
#include "Package/Package.h"

/// <summary>
/// �v���O�������s���ŏ��ɌĂяo�����֐�
/// </summary>
/// <returns>�֐�������ɏI���������ǂ���</returns>
#ifdef _DEBUG
int main() {
#else
#include <Windows.h>
int WINAPI WinMain(_In_ HINSTANCE,_In_opt_ HINSTANCE,_In_ LPSTR,_In_ int) {
#endif

	auto& app = Application::Instance();	//Application�C���X�^���X�擾

	if (!app.Init())						//Application�C���X�^���X�̏��������������Ȃ���Έُ�I��
	{
		return -1;
	}

	app.Run();								//�Q�[����ʂ�`��
	app.Terminate();						//�I�����̌�n��

	return 0;								//����I��
}