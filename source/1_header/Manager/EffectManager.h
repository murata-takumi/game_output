#pragma once
#include "Application.h"

#include <EffekseerRendererDX12/Effekseer.h>
#include <EffekseerRendererDX12/EffekseerRendererDX12.h>

using namespace EffekseerRendererDX12;

/// <summary>
/// �G�t�F�N�g���Ǘ�����N���X
/// </summary>
class Dx12Wrapper;
class EffectManager
{
private:

	Dx12Wrapper& _dx12;																			//Dx12Wrapper�C���X�^���X

	EffekseerRenderer::RendererRef _efkRenderer = nullptr;										//�G�t�F�N�g�����_���[

	Effekseer::ManagerRef _efkManager = nullptr;												//�G�t�F�N�g�}�l�[�W���[

	//Effekseer��DX12�Ŏg�p�ł���悤�ɂ��邽�߂̕ϐ�
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> _efkMemoryPool = nullptr;		//�������v�[��

	Effekseer::RefPtr<EffekseerRenderer::CommandList> _efkCmdList = nullptr;					//�R�}���h���X�g

	//�G�t�F�N�g�Đ��ɕK�v�Ȃ���
	Effekseer::EffectRef _effect = nullptr;														//�G�t�F�N�g�{�́i�G�t�F�N�g�t�@�C���ɑΉ��j

	Effekseer::Handle _efkHandle;																//�G�t�F�N�g�n���h���i�Đ����̃G�t�F�N�g�ɑΉ��j

public:

	EffectManager(Dx12Wrapper& dx12);		//�R���X�g���N�^

	void CallEffect();						//�G�t�F�N�g���J�n����֐�

	void Draw();							//�G�t�F�N�g���X�V����֐�
};