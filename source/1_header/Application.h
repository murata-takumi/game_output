#pragma once
#include "Includes.h"

#if _DEBUG

#pragma comment(lib,"assimp-vc143-mtd.lib")
#pragma comment(lib,"EffekseerRendererDX12d.lib")
#pragma comment(lib,"Effekseerd.lib")
#pragma comment(lib,"LLGId.lib")

#else

#pragma comment(lib,"assimp-vc143-mt.lib")
#pragma comment(lib,"EffekseerRendererDX12.lib")
#pragma comment(lib,"Effekseer.lib")
#pragma comment(lib,"LLGI.lib")

#endif // DEBUG


#pragma comment(lib,"DirectXTK12.lib")
#pragma comment(lib,"DirectXTex.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Imgui.lib")

/// <summary>
/// �ǂݍ���Scene�N���X�����ʂ��邽�߂̗񋓌^
/// </summary>
enum SceneNames
{
	Title,		//�^�C�g���V�[��
	Play,		//�Q�[���V�[��
};

/// <summary>
/// FBX���f���̒��_�p�\����
/// </summary>
struct FBXVertex
{
	XMFLOAT3 position;					//���W
	XMFLOAT3 normal;					//�@��
	XMFLOAT2 uv;						//UV���W
	XMFLOAT3 tangent;					//����
	XMFLOAT4 color;						//���_�J���[
	XMUINT4 ids;						//�{�[��ID
	XMFLOAT4 weights;					//�{�[���̉e���l
};

class Dx12Wrapper;
class EffectManager;
class InputManager;
class Package;
class PlayScene;
class SoundManager;
/// <summary>
/// �Q�[���̏������E�X�V�E�I�����Ǘ�����N���X
/// </summary>
class Application
{
private:
	//�E�B���h�E�쐬���ɕK�v�ȏ����i�[
	WNDCLASSEX _windowClass;								
	//�E�B���h�E�̎��ʂɕK�v�Ȓl
	HWND _hwnd;														

	//Dx12Wrapper�C���X�^���X
	shared_ptr<Dx12Wrapper> _dx12;								
	//SoundManager�C���X�^���X
	shared_ptr<SoundManager> _sound;								
	//EffectManager�C���X�^���X
	shared_ptr<EffectManager> _effect;								
	
	//PlayScene�C���X�^���X
	shared_ptr<PlayScene> _play;									

	//�Q�[�����Ŏg�p����e��f�[�^���Ǘ�����C���X�^���X
	shared_ptr<Package> _package;									

	//�t���[���Ԃ̌o�ߎ���
	float _deltaTime;				
	//1�b������̃t���[����
	int _rate;														
	//�t���b�v�Ԋu
	int _interval;													

	//�Q�[���p�E�B���h�E���쐬����֐�
	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		

	//�R���X�g���N�^
	Application();													
	//�R���X�g���N�^���O������Ăяo����Ȃ��悤�ݒ�
	Application(const Application&) = delete;						

public:

	//���b�Z�[�W�p�\����
	MSG _msg = {};									

	//�C���X�^���X�̎Q�Ƃ�Ԃ��H
	static Application& Instance();					

	//������
	bool Init();									

	//�Q�[����ʂ̕`��
	void Run();										

	//�Q�[���I�����̌�n��
	void Terminate();								

	//�E�B���h�E�T�C�Y��Ԃ�
	SIZE GetWindowSize()const;						

	//�A�v���P�[�V�������I������
	void ExitApp();									

	//�����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��Ԃ��֐�
	int GetInterval()const;							
	//1�b�Ԃ̃t���[�����[�g��Ԃ��֐�
	int GetRate()const;								

	//�f�X�g���N�^
	~Application();									
};
