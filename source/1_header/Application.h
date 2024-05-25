#pragma once
#include <DirectXTex-master/DirectXTex/d3dx12.h>
#include <DirectXTex-master/DirectXTex/DirectXTex.h>
#include <DirectXTK12-master/Inc/SimpleMath.h>

#include <d3dcompiler.h>
#include <array>
#include <cstdlib>
#include <map>
#include <tchar.h>
#include <dxgi1_6.h>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

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

const float PI = 3.141592653f;		//�~����

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
	WNDCLASSEX _windowClass;										//�E�B���h�E�쐬���ɕK�v�ȏ����i�[
	HWND _hwnd;														//�E�B���h�E�̎��ʂɕK�v�Ȓl

	shared_ptr<Dx12Wrapper> _dx12;									//Dx12Wrapper�C���X�^���X
	shared_ptr<InputManager> _input;								//InputManager�C���X�^���X	
	shared_ptr<SoundManager> _sound;								//SoundManager�C���X�^���X
	shared_ptr<EffectManager> _effect;								//EffectManager�C���X�^���X
	
	shared_ptr<PlayScene> _play;									//PlayScene�C���X�^���X

	shared_ptr<Package> _package;									//�Q�[�����Ŏg�p����e��f�[�^���Ǘ�����C���X�^���X

	float _deltaTime;												//�t���[���Ԃ̌o�ߎ���

	int _rate;														//1�b������̃t���[����
	int _interval;													//�t���b�v�Ԋu

	void CreateGameWindow(HWND& hwnd, WNDCLASSEX& windowClass);		//�Q�[���p�E�B���h�E���쐬����֐�

	Application();													//�R���X�g���N�^
	Application(const Application&) = delete;						//�R���X�g���N�^���O������Ăяo����Ȃ��悤�ݒ�

public:

	MSG _msg = {};									//���b�Z�[�W�p�\����

	static Application& Instance();					//�C���X�^���X�̎Q�Ƃ�Ԃ��H

	bool Init();									//������

	void Run();										//�Q�[����ʂ̕`��

	void Terminate();								//�Q�[���I�����̌�n��

	SIZE GetWindowSize()const;						//�E�B���h�E�T�C�Y��Ԃ�

	void ExitApp();									//�A�v���P�[�V�������I������

	int GetInterval()const;							//�����_�[�^�[�Q�b�g�̃t���b�v�Ԋu��Ԃ��֐�
	int GetRate()const;								//1�b�Ԃ̃t���[�����[�g��Ԃ��֐�

	~Application();									//�f�X�g���N�^
};
