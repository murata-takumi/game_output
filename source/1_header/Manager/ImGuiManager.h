#pragma once
#include "Includes.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"

class Dx12Wrapper;
class FBXBase;
class FBXActor;
class ImGuiManager
{
public:
	ImGuiManager(Dx12Wrapper& dx12,HWND hwnd);						//�R���X�g���N�^

	bool CanActorTrans()const;										//�A�N�^�[���ړ��\���ǂ�����Ԃ��֐�
	bool IsWriteGrid()const;										//�O���b�h��`�悷�邩�ǂ�����Ԃ��֐�

	void ImGuiDraw();												//�e�E�B���h�E�̕`�揈��

	void SetFPS(float fps);											//PlayScene����󂯎�����f�[�^�𔽉f����֐�
	//�A�N�^�[�ƒn�ʂ̍��W���Ƃ̐^���l�̓����蔻����󂯎��
	void SetActorColBool(bool intersects[]);
	void SetActor(vector<shared_ptr<FBXBase>> actorAndObjects);		//�A�N�^�[���󂯎��֐�
	void ResetAnimStr();											//�A�j���[�V�������̔z�������������֐�

private:
	Application& _app;								//Application�C���X�^���X
	Dx12Wrapper& _dx12;								//Dx12Wrapper�C���X�^���X

	shared_ptr<FBXActor> _actor;					//�A�N�^�[
	shared_ptr<FBXBase> _ground;					//�n��

	ComPtr<ID3D12DescriptorHeap> _heapForImgui;		//ImGui�p�q�[�v

	string _currentImGuiAnim;						//�A�N�^�[�����ݎ��s���Ă���A�j���[�V������
	vector<string> _animStr;						//�S�A�j���[�V������

	ImGuiWindowFlags _fpsFlag = 0;					//FPS�\���p�E�B���h�E�̐ݒ�t���O
	ImGuiWindowFlags _animFlag = 0;					//�A�j���[�V�����\���p�E�B���h�E�̐ݒ�t���O

	LARGE_INTEGER _beforeFrameTime;					//�O�t���[���̎���
	LARGE_INTEGER _crrntFrameTime;					//���t���[���̎���

	//�A�N�^�[�ƒn�ʂ̍��W���Ƃ̓����蔻���\������^���l
	bool _intersects[6];

	bool _canActorControll = false;					//����\���ǂ��������߂�^���l
	bool _comboSelect = false;						//�A�j���[�V�����̃R���{�{�b�N�X�̓��ǂꂪ�I������Ă��邩�����߂�^���l
	bool _canCallAnim = false;						//�A�j���[�V���������s���邩�ǂ��������߂�^���l
	bool _isWriteGrid = true;						//�O���b�h��`�悷�邩�ǂ������߂�^���l

	float _windowWidth;								//�E�B���h�E��
	float _windowHeight;							//�E�B���h�E����

	float _size;									//�t�H���g�T�C�Y
	float _speed;									//�A�j���[�V�����̍Đ��X�s�[�h
	float _tick;									//

	float _maxDur;									//�A�j���[�V�����̑�����
	float _animSliderValue;							//�A�j���[�V�����̌��ݎ���

	float _fps;										//1�b������̃t���[�����[�g

	void Update();									//�`��ȊO�̏��������s����֐�
};