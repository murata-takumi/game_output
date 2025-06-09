#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Wrapper/Dx12Wrapper.h"

//�A�j���[�V�������ɕt�^���镶���񃊃e����
const char* CHARA_REF = "Character1_Reference|";

//�ړ����x
const float MOVE_SPEED = 300.0f;
//�d�͉����x
const float GRAVITY_ACCERALATION = 9.8f;
//�A�j���[�V�����̃u�����h�Ɋ|����b��
const float BLEND_SPEED = 0.2f;
//�A�j���[�V�����̊J�n����
const float START_TIME = 8.5f;
//�ʃA�j���[�V�����ւ̑J�ڂ��o����悤�ɂȂ鎞��
const float SELECT_NEXT_ANIM_TIME = 30.0f;
const float TRANSITION_TIME = 34.5f;

//���[�v�̊J�n�A�I������
const float LOOP_BEGIN_TIME = 30.0f;
const float LOOP_END_TIME = 31.0f;
//�A�j���[�V�������x
const float ANIM_SPEED = 0.1f;

//�A�j���[�V�������̕s�v�ȕ�������폜���邽�߂̃C���f�b�N�X
const int ANIM_STR_UNNECESSARY_IDX = 21;
//�����蔻���t�^����{�[���̃C���f�b�N�X
const int COLLIDER_BONE = 0;								

/// <summary>
/// �R���X�g���N�^
/// </summary>
/// <param name="filePath">���f���̃p�X</param>
FBXActor::FBXActor(const wchar_t* filePath,const string name, Vector3 size, Vector3 pos)
	:FBXBase(filePath, name, size, pos),
	_crntNode(nullptr), _currentFrontVec(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
	_canControll(false), _isInBlend(false), _isInLoop(true),
	_canChangeAnim(true),_blendWeight(0.0f), _animTime(0.0f),
	_destRad(0.0f), _rotY(0.0f)
{
	//�ڒn�p�����蔻����쐬
	_colForGround = make_shared<BoxCollider>(Vector3(size.X() * 2, size.Y() * 2, size.Z() * 2), Vector3(0, 0, 0), this);
	auto jumpStart = [&]()
	{
		//�J�n���Ԃ�������ɐݒ�
		SetAnimationTime(START_TIME);

		//�ʃA�j���[�V�����ɑJ�ڂ��Ȃ��悤�ɂ���
		SetCanChangeAnim(false);
		//���[�v���Ȃ��悤�ɂ���
		SetIsInLoop(false);
		//�����蔻�肪�{�[���ϊ��̉e�����󂯂�悤�ɂ���
		SetRejectBone(false);
	};
	auto jumpUpdate = [&](float animTime)
	{
		//�A�j���[�V�������Ԃ�30�b�̎��_�ő����ɓ����蔻�肪���邩�ǂ�������
		AnimEnum nextAnimName;
		if (animTime > SELECT_NEXT_ANIM_TIME)
		{
			if (GetOnGround())
			{
				//���͂���Ă��邩�ǂ����őJ�ڐ�̃A�j���[�V���������߂�
				if (XMVector3Length(_inputVec).m128_f32[0] > 0.0f)
				{
					//�ړ��A�j���[�V����
					nextAnimName = RUN00_F;
				}
				else
				{
					//�ҋ@�A�j���[�V����
					nextAnimName = WAIT00;
				}
			}
			else
			{
				//�����A�j���[�V����
				nextAnimName = FALL;
			}

		}
		if (animTime > TRANSITION_TIME)
		{
			SetCanChangeAnim(true);
			//�����蔻�肪�{�[���ϊ��̉e�����󂯂Ȃ��悤�ɂ���
			SetRejectBone(true);

			SetAnimationNode(nextAnimName);
		}
	};
	auto jumpEnd = [&]()
	{		
		//���[�v�ł���悤�ɂ���
		SetIsInLoop(true);
	};

	auto fallStart = [&]()
	{
		//�J�n���Ԃ�������ɐݒ�
		SetAnimationTime(LOOP_BEGIN_TIME);

		//�ʃA�j���[�V�����ɑJ�ڂ��Ȃ��悤�ɂ���
		SetCanChangeAnim(false);
		SetAnimationSpeed(ANIM_SPEED);
	};
	auto fallUpdate = [&](float animTime)
	{
		//���[�v���Ă���悤�Ɍ�����ׂ̋���̍�
		if (animTime <= LOOP_BEGIN_TIME)
		{
			animTime = LOOP_BEGIN_TIME;
			SetAnimationSpeed(ANIM_SPEED);
		}
		else if (LOOP_END_TIME < animTime)
		{
			animTime = LOOP_END_TIME;
			SetAnimationSpeed(-ANIM_SPEED);
		}

		//�n�ʂɒ�������ʃA�j���[�V�����ɑJ�ڂł���悤�ɂ���
		if (GetOnGround())
		{
			SetCanChangeAnim(true);
		}
	};
	auto fallEnd = [&]()
	{
		//�A�j���[�V�������x�����ɖ߂�
		SetAnimationSpeed(1.0f);
	};
	

	//�A�j���[�V�����m�[�h��������
	_animNodes[WAIT00] = make_unique<AnimNode>(WAIT00);
	_animNodes[RUN00_F] = make_unique<AnimNode>(RUN00_F);
	_animNodes[JUMP00] = make_unique<AnimNode>(JUMP00,jumpStart,jumpUpdate,jumpEnd);
	_animNodes[FALL] = make_unique<AnimNode>(FALL, fallStart, fallUpdate, fallEnd);

	//�A�j���[�V�����Ɋւ���f�[�^��������
	InitAnimation();															

	//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬
	CreateTransformView();														
}

/// <summary>
/// �f�X�g���N�^
/// �|�C���^���J������
/// </summary>
FBXActor::~FBXActor()
{

}

/// <summary>
/// �A�j���[�V�����֘A�̏��������s���֐�
/// </summary>
void
FBXActor::InitAnimation()
{
	//�{�[���s��̌������߂�
	_beforeMats.resize(_boneMats.size());
	_boneMats.resize(_boneInfo.size());		
	//�u�����h�p�̍s��̌������߂�
	_blendMats.resize(_boneInfo.size());							
	//�t�ϊ��p�̍s��̌������߂�
	_invMats.resize(_boneInfo.size());

	//�e�{�[���s��̃X�P�[�����O�E���s�ړ��𖳌��������s����쐬
	for (UINT i = 0; i < _invMats.size(); i++)
	{
		//�{�[���s��𕪉����A�N�H�[�^�j�I����X����-90�x�AY����90�x��]
		XMVECTOR scale, skew, trans;
		XMMatrixDecompose(&scale, &skew, &trans, 
			XMMatrixInverse(nullptr, _boneInfo[i]._boneOffset));

		auto q1 = XMQuaternionRotationAxis(							
			XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PI / -2);
		auto q2 = XMQuaternionRotationAxis(
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XM_PI / 2);
		skew = XMQuaternionMultiply(skew, q1);
		skew = XMQuaternionMultiply(skew, q2);

		//�s����č���
		XMMATRIX invMat = XMMatrixScalingFromVector(scale) *		
						XMMatrixRotationQuaternion(skew) * 
						XMMatrixTranslationFromVector(trans);

		//�s��x�N�g���Ɋi�[
		_invMats[i] = invMat;										
	}

	//���f�������S�A�j���[�V�����ɑ΂����s
	for (UINT i = 0; i < FBXBase::_scene->mNumAnimations; i++)				
	{
		//�A�z�z��ɃA�j���[�V�������ƃA�j���[�V�����f�[�^���i�[
		_anims[FBXBase::_scene->mAnimations[i]->mName.C_Str()]
			= FBXBase::_scene->mAnimations[i];

		//�A�j���[�V���������擾
		string name = FBXBase::_scene->mAnimations[i]->mName.C_Str();

		//�]���ȕ�������폜
		name = name.erase(0, ANIM_STR_UNNECESSARY_IDX);									

		//�A�j���[�V�������̃x�N�g���ɒǉ�
		_animStr.push_back(name);									
	}

	//�ŏ��Ɏ��s����A�j���[�V������ݒ�
	_currentActorAnim = _animStr[0];								

	//T�|�[�Y�ɂ���
	InitPose();														
}

/// <summary>
/// �{�[���ϊ��s����������ފ֐�
/// </summary>
/// <param name="timeInTicks">���݂̌o�ߎ���</param>
void
FBXActor::BoneTransform(float timeInTicks)
{
	//�o�ߎ��Ԃ���ɊK�w�\������ϊ��s���ǂݍ���
	if (timeInTicks < GetAnimDuration(_currentActorAnim))									
	{
		ReadNodeHeirarchy(timeInTicks, FBXBase::_scene->mRootNode, XMMatrixIdentity());
	}

	//�ϊ��s����X�V����
	for (size_t i = 0,boneLength = _boneMats.size(); i < boneLength; ++i)					
	{
		_boneMats[i] = _boneInfo[i]._finalTrans;
	}
}

/// <summary>
/// �A�j���[�V�����m�[�h��ݒ肷��֐�
/// </summary>
/// <param name="anim">�Ăяo�������A�j���[�V����</param>
void
FBXActor::SetAnimationNode(AnimEnum anim)
{
	//�A�j���[�V������ύX�ł����Ԃ��m�F
	//�����A�j���[�V�����̍Đ����悤�Ƃ��������֐����I��������
	if (!_canChangeAnim || IsAnimationEqual(anim)) return;

	//�O�m�[�h�̏I������
	if (_crntNode != nullptr)
	{
		_crntNode->EndAnimation();		
	}

	//�m�[�h�X�V
	_crntNode = _animNodes[anim].get();
	BlendAnimation(_crntNode->GetAnimEnum());
	//���m�[�h�̊J�n����
	_crntNode->StartAnimation();
}

/// <summary>
/// �ċA�I�Ƀm�[�h�̊K�w��ǂݍ��݁A���W�ϊ����s���s����擾����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="pNode">�e�m�[�h</param>
/// <param name="parentTrans">�e�m�[�h�œK�p����Ă���s��</param>
void
FBXActor::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const XMMATRIX& parentTrans)
{
	//�m�[�h��
	string nodeName(pNode->mName.data);													
	//�A�j���[�V�������擾
	const aiAnimation* pAnimation = _anims[CHARA_REF + _currentActorAnim];				
	//�m�[�h��aiMatrix4x4��XMMATRIX�ɕϊ�
	aiMatrix4x4 aiTrans = pNode->mTransformation;										
	XMFLOAT4X4 temp = XMFLOAT4X4
	(
		aiTrans.a1, aiTrans.b1, aiTrans.c1, aiTrans.d1,
		aiTrans.a2, aiTrans.b2, aiTrans.c2, aiTrans.d2,
		aiTrans.a3, aiTrans.b3, aiTrans.c3, aiTrans.d3,
		aiTrans.a4, aiTrans.b4, aiTrans.c4, aiTrans.d4
	);
	XMMATRIX nodeTrans = XMLoadFloat4x4(&temp);
	//�m�[�h���Ɠ�����aiNodeAnim�����o��
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);					

	//�A�j���[�V����������ꍇ�e�s����擾����
	if (pNodeAnim != nullptr)															
	{
		//�X�P�[�����O
		XMMATRIX scalingM = CalcInterpolatedScaling(animationTime, pNodeAnim);
		//��]
		XMMATRIX rotationM = CalcInterpolatedRotation(animationTime, pNodeAnim);		
		//���s�ړ�
		XMMATRIX translationM = CalcInterpolatedPosition(animationTime, pNodeAnim);		
		//�s�������
		nodeTrans = scalingM * rotationM * translationM;								
	}

	//�e�̕ϊ��s���K�p
	XMMATRIX globalTrans = nodeTrans * parentTrans;										

	//�{�[�����ɑΉ�����C���f�b�N�X���擾���A�������ɍŏI�I�ȕϊ��s����擾
	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		unsigned int boneIdx = _boneMapping[nodeName];

		_boneInfo[boneIdx]._finalTrans = _boneInfo[boneIdx]._boneOffset * globalTrans;
	}

	//�q�m�[�h�̕��ɂ��ċA�������s��
	auto numChildren = pNode->mNumChildren;
	for (UINT i = 0; i < numChildren; i++)												
	{
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTrans);
	}
}

/// <summary>
/// �I�u�W�F�N�g�̍��W�ϊ��ɗp������q�[�v�E�r���[���쐬����֐�
/// </summary>
/// <returns>�쐬�ł������ǂ���</returns>
HRESULT
FBXActor::CreateTransformView()
{
	result = S_OK;

	//���[���h�s��p�o�b�t�@�[�̍쐬
	auto buffSize = sizeof(XMMATRIX) * (1 + _boneMats.size());						
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = Dx12Wrapper::Instance().Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(FBXBase::_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//���W�ϊ��p�s��̏������݁A�������W�Ɉʒu����悤�ɂ���
	result = FBXBase::_transBuffer->Map(0, nullptr, (void**)&(FBXBase::_mappedMats));
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	FBXBase::_mappedMats[0] = XMMatrixIdentity();
	FBXBase::_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);
	copy(_boneMats.begin(), _boneMats.end(), FBXBase::_mappedMats + 1);

	//�f�B�X�N���v�^�q�[�v�̍쐬
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	result = Dx12Wrapper::Instance().Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(FBXBase::_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//�r���[�̍쐬
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = FBXBase::_transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(FBXBase::_transBuffer->GetDesc().Width);
	Dx12Wrapper::Instance().Device()->CreateConstantBufferView(&cbvDesc,								
		FBXBase::_transHeap->GetCPUDescriptorHandleForHeapStart());

	//�����蔻���������
	FBXBase::_collider->Update(FBXBase::_mappedMats[0]);

	return result;
}

/// <summary>
/// �A�j���[�V�����z��̒����疼�O����v�����A�j���[�V���������o���֐�
/// </summary>
/// <param name="animation">�A�j���[�V�����z��</param>
/// <param name="nodeName">�A�j���[�V����</param>
/// <returns>�m�[�h������v�����A�j���[�V����</returns>
const aiNodeAnim*
FBXActor::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		//aiNodeAnim�̖��O���擾�������̃A�j���[�V�������ƈ�v���Ă����炻���Ԃ�
		const aiNodeAnim* nodeAnim = animation->mChannels[i];	
		if (string(nodeAnim->mNodeName.data) == nodeName)
		{
			return nodeAnim;
		}
	}

	//�Ȃ�������nullptr
	return nullptr;												
}

/// <summary>
/// �X�P�[�����O�̕�Ԃ����{����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>�X�P�[�����O�s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	//�X�P�[�����O�ŗp����L�[��1�̏ꍇ�A���̃L�[�̃x�N�g�������̂܂܎擾
	aiVector3D tempVec;
	if (nodeAnim->mNumScalingKeys == 1)											
	{
		tempVec = nodeAnim->mScalingKeys[0].mValue;
	}
	//�����łȂ��ꍇ�A���t���[���Ǝ��t���[������x�N�g�������߂�
	else
	{
		//�o�ߎ��Ԃ���ɃL�[�̃C���f�b�N�X�Ǝ��̃L�[�̃C���f�b�N�X���擾
		int scalingIdx = FindScaling(animationTime, nodeAnim);
		UINT nextScalingIdx = scalingIdx + 1;
		assert(nextScalingIdx < nodeAnim->mNumScalingKeys);

		//�L�[���m�̌o�ߎ��Ԃ̍������擾
		float deltaTime = static_cast<float>(
			nodeAnim->mScalingKeys[nextScalingIdx].mTime
			- nodeAnim->mScalingKeys[scalingIdx].mTime);

		//���ۂ̌o�ߎ��Ԃ̃L�[���m�̍����ɑ΂���{�������߂�
		float factor =
			(animationTime
				- static_cast<float>(nodeAnim->mScalingKeys[scalingIdx].mTime)
				)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//���t���[���A���t���[���̃x�N�g���̍��������߂�
		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIdx].mValue;
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIdx].mValue;
		aiVector3D delta = end - start;

		//�x�N�g���̍����ɔ{���������A��������t���[���̃x�N�g���ɉ��Z
		tempVec = start + factor * delta;
	}

	////aiVector3D����XMVector�֕ϊ����A�X��XMMatrix�ɕϊ�
	Vector3 scaling(XMVectorSet(tempVec.x, tempVec.y, tempVec.z, 0));
	XMMATRIX ret = XMMatrixScalingFromVector(scaling);

	return ret;
}

/// <summary>
/// ��]�̕⊮�����s����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>��]�s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiQuaternion tempQuat;

	//�L�[��������Ȃ��ꍇ���̃L�[�̃N�H�[�^�j�I�����擾
	if (nodeAnim->mNumRotationKeys == 1)														
	{
		tempQuat = nodeAnim->mRotationKeys[0].mValue;
	}
	else
	{
		//�o�ߎ��Ԃ���ɃL�[�Ǝ��̃L�[�̃C���f�b�N�X���擾
		int rotationIdx = FindRotation(animationTime, nodeAnim);								
		UINT nextRotationIdx = rotationIdx + 1;
		assert(nextRotationIdx < nodeAnim->mNumRotationKeys);

		//2�̃L�[�̎��Ԃ̍������擾
		float deltaTime = static_cast<float>													
			(
				nodeAnim->mRotationKeys[nextRotationIdx].mTime -
				nodeAnim->mRotationKeys[rotationIdx].mTime
				);

		//�L�[�̎��Ԃ̍����ɑ΂���o�ߎ��Ԃ��玟�̃L�[�̎��Ԃ̊������擾
		float factor =																			
			(animationTime - static_cast<float>(nodeAnim->mRotationKeys[rotationIdx].mTime))
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//�L�[�Ǝ��̃L�[�̃N�H�[�^�j�I��
		const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIdx].mValue;				
		const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIdx].mValue;

		//��Ԃ��Đ��K��
		aiQuaternion::Interpolate(tempQuat, start, end, factor);								
		tempQuat = tempQuat.Normalize();
	}

	//�N�H�[�^�j�I������aiMatrix3x3���擾�A�����XMMatrix�ɕϊ�
	aiMatrix3x3 rotMat = tempQuat.GetMatrix();													
	auto rotTemp = XMFLOAT3X3
	(
		rotMat.a1, rotMat.b1, rotMat.c1,
		rotMat.a2, rotMat.b2, rotMat.c2,
		rotMat.a3, rotMat.b3, rotMat.c3
	);
	XMMATRIX ret = XMLoadFloat3x3(&rotTemp);

	return ret;
}

/// <summary>
/// ���s�ړ��̕�Ԃ����s����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V����</param>
/// <returns>���s�ړ��s��</returns>
XMMATRIX
FBXActor::CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempPos;

	//�L�[���������������Ή�����ړ��x�N�g�������̂܂܎擾
	if (nodeAnim->mNumPositionKeys == 1)											
	{
		tempPos = nodeAnim->mPositionKeys[0].mValue;
	}
	else
	{
		//�o�ߎ��Ԃ���ɃL�[�̃C���f�b�N�X���擾
		int positionIdx = FindPosition(animationTime, nodeAnim);					
		UINT nextPositionIdx = positionIdx + 1;
		assert(nextPositionIdx < nodeAnim->mNumPositionKeys);

		//��̃L�[�̎��Ԃ̍������擾
		float deltaTime =															
			static_cast<float>(
				nodeAnim->mPositionKeys[nextPositionIdx].mTime - 
				nodeAnim->mPositionKeys[positionIdx].mTime
			);
		//�L�[�̎��Ԃ̍����ɑ΂���o�ߎ��Ԃ��玟�̃L�[�̍����̊��������߂�
		float factor =																
			(
				animationTime - 
				static_cast<float>(nodeAnim->mPositionKeys[positionIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//��̃L�[�ɑΉ�����ړ��x�N�g���̍������擾���A������p���ĕ⊮����
		const aiVector3D& start = nodeAnim->mPositionKeys[positionIdx].mValue;
		const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIdx].mValue;
		aiVector3D delta = end - start;
		tempPos = start + factor * delta;
	}
	//aiVector3D����XMVector�ɕϊ����A���̌�XMMatrix�ɕϊ�
	Vector3 translation(XMVectorSet(tempPos.x, tempPos.y, tempPos.z, 0));			
	XMMATRIX ret = XMMatrixTranslationFromVector(translation);

	return ret;
}

/// <summary>
/// �X�P�[�����O�A�j���[�V�����̃L�[�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime">���݂̌o�ߎ���</param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int
FBXActor::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numSclKeys = nodeAnim->mNumScalingKeys;
	assert(numSclKeys > 0);

	//���[�v���񂵁A�o�ߎ��Ԃ�����L�[��Ԃ�
	for (UINT i = 0; i < numSclKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// ��]�Ɋւ���A�j���[�V�����f�[�^�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int 
FBXActor::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numRotKeys = nodeAnim->mNumRotationKeys;
	assert(numRotKeys > 0);

	//���[�v���񂵁A�o�ߎ��Ԃ�����L�[��Ԃ�
	for (UINT i = 0; i < numRotKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// ���s�ړ��Ɋւ���A�j���[�V�����f�[�^�̃C���f�b�N�X���擾����֐�
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">�A�j���[�V�����f�[�^</param>
/// <returns>�C���f�b�N�X</returns>
int
FBXActor::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numPosKeys = nodeAnim->mNumPositionKeys;
	assert(numPosKeys > 0);

	//���[�v���񂵁A�o�ߎ��Ԃ�����L�[��Ԃ�
	for (UINT i = 0; i < numPosKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// �A�N�^�[��T�|�[�Y�ɂ���֐�
/// </summary>
void
FBXActor::InitPose()
{
	_secFrameTime = _anims[CHARA_REF + _currentActorAnim]->mChannels[0]->mPositionKeys[1].mTime;
}

/// <summary>
/// ���t���[���̍��W�ϊ����������s����֐�
/// </summary>
void
FBXActor::Update()
{
	FBXBase::Update();

	//����\�ȏꍇ
	if (_canControll)			
	{
		//���t���[���̎��Ԃ��擾���O�t���[���̎��ԂƂ̍������o�ߎ��Ԃɉ��Z
		QueryPerformanceCounter(&_currFrameTime);
		auto diff = GetLIntDiff(_currFrameTime, _befFrameTime);
		_animTime += static_cast<float>(diff) * GetAnimTickPerSpeed(_currentActorAnim) * _animSpeed;

		_colForGround->Update(_motionMat * _mappedMats[0]);

		//�ŏ��̃t���[���𖳎����������ŃA�j���[�V���������[�v����悤�ݒ�
		if (_isInLoop)
		{
			_animTime = fmod(_animTime, GetAnimDuration(_currentActorAnim));		
			_animTime = clamp(_animTime,GetSecondFrame(), GetAnimDuration(_currentActorAnim));
		}

		//�A�j���[�V�����m�[�h�̍X�V
		_crntNode->Update(_animTime);												

		//�n�ʂ̏�ɂ��Ȃ������痎������
		if (!GetOnGround() && !IsAnimationEqual(JUMP00))
		{
			_pos.Y() -= Dx12Wrapper::Instance().GetDeltaTime() * 45.0f * GRAVITY_ACCERALATION;
			FBXBase::_speed.Y() = -45.0f * GRAVITY_ACCERALATION;
			SetAnimationNode(FALL);
		}

		//��]�A���s�ړ�
		FBXBase::_mappedMats[0] = XMMatrixRotationY(_rotY);
		FBXBase::_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);
	}

	//�X�P�[�����O�A��]�A���s�ړ�������ϊ��s��ƃ{�[���s��̐ς���擾���A��]�v�f���폜���������ōēx����
	XMVECTOR scale, trans, skew;													
	XMMatrixDecompose(&scale, &skew, &trans, 
		_invMats[COLLIDER_BONE] * FBXBase::_mappedMats[COLLIDER_BONE + 1]);
	skew = XMVectorZero();
	if(!_rejectBone)_motionMat = XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(skew) * XMMatrixTranslationFromVector(trans);

	//�o�ߎ��Ԃ�n���A�{�[���s����擾
	BoneTransform(_animTime);														
	
	//�u�����h�s��or���ʂ̍s����V�F�[�_�[�ɓn���A�A�j���[�V���������s
	if (_isInBlend)	copy(_blendMats.begin(), _blendMats.end(), FBXBase::_mappedMats + 1);
	else copy(_boneMats.begin(), _boneMats.end(), FBXBase::_mappedMats + 1);

	//�O�t���[���̎��Ԃ��X�V
	_befFrameTime = _currFrameTime;
}

/// <summary>
/// ���͂��ꂽ�����ֈړ�����֐�
/// </summary>
/// <param name="input">���̓x�N�g��</param>
/// <param name="canTrans">�ړ��ł��邩�ǂ���</param>
void
FBXActor::Translate(const Vector3& input, bool canTrans)
{
	//���͂���Ă��邩�ǂ����ɉ����čĐ�����A�j���[�V���������߂�
	if (XMVector3Length(input).m128_f32[0] > 0.0f)
	{
		//�ړ��A�j���[�V����
		SetAnimationNode(RUN00_F);
	}
	else
	{
		//�ҋ@�A�j���[�V����
		SetAnimationNode(WAIT00);
	}

	//���̓x�N�g�����X�V
	_inputVec = input;

	if (canTrans)
	{
		//���W�ɓ��͂ɉ������x�N�g�������Z
		_pos += input * Dx12Wrapper::Instance().GetDeltaTime() * MOVE_SPEED;

		FBXBase::_speed.X() = input.X() * MOVE_SPEED;
		FBXBase::_speed.Z() = input.Z() * MOVE_SPEED;
	}

	//���̓x�N�g���Ɛ��ʃx�N�g���̊p�x�����擾
	auto diff = XMVector3AngleBetweenVectors(input, _currentFrontVec).m128_f32[0];			
	diff = XMVector3Cross(input, _currentFrontVec).m128_f32[1] > 0 ? diff * -1 : diff;
	//�p�x�̍�������ꍇ�ړI�p�x���X�V
	if (FLT_EPSILON <= fabs(diff))													
	{
		_destRad = _rotY + diff;
	}

	//�ړI�p�x�Ɍ������`���
	_rotY = lerp(_rotY, _destRad, 0.2f);											

	//Z�����琳�ʃx�N�g���̊p�x�����擾
	//0�`360�͈̔͂ɂ���
	_zToFrontAngle = XMVector3AngleBetweenVectors(Z_VECTOR, _currentFrontVec).m128_f32[0];
	_zToFrontAngle = XMVector3Cross(Z_VECTOR, _currentFrontVec).m128_f32[1] > 0 ? (2 * XM_PI) - _zToFrontAngle : _zToFrontAngle;
	_zToFrontAngle = _zToFrontAngle / XM_PI * 180.0f;

	//���ʃx�N�g�����擾�E���K��
	_currentFrontVec = XMVectorSet(														
		Vector3(FBXBase::_mappedMats[0].r[2]).X(),
		Vector3(FBXBase::_mappedMats[0].r[2]).Y(),
		Vector3(FBXBase::_mappedMats[0].r[2]).Z(),
		0.0f
	);
	_currentFrontVec = XMVector3Normalize(_currentFrontVec);
}

/// <summary>
/// ����\���ǂ�����ݒ肷��֐�
/// </summary>
void
FBXActor::StartControll()
{
	//�A�j���[�V�����Đ����Ԃ�������
	_animTime = 0.0f;				
	//�^���l��ݒ�
	_canControll = true;			
	//�A�j���[�V������ҋ@���[�V�����ɐݒ�
	SetAnimationNode(WAIT00);		
}

/// <summary>
/// ���W������������֐�
/// </summary>
void
FBXActor::EndControll()
{
	//�P�ʍs��������A���W�␳�ʃx�N�g����������
	FBXBase::_mappedMats[0] = XMMatrixIdentity();
	_pos = XMVectorZero();
	_currentFrontVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	//�p�x�A�ړI�p�x��������
	_rotY = 0.0f;
	_destRad = 0.0f;

	//�^���l��������
	_canControll = false;					
	//�|�[�Y��������
	InitPose();											
}

/// <summary>
/// �A�j���[�V�����Ԃ̃u�����h�����s����֐�
/// </summary>
/// <param name="anim">�A�j���[�V����</param>
void
FBXActor::BlendAnimation(AnimEnum anim)
{
	//�u�����h�p�ɑO�A�j���[�V�����̍s���ۑ�
	_beforeMats = _boneMats;

	//�A�j���[�V������ύX
	_currentActorAnim = animations[anim];

	//�Đ����Ԃ�2�ڂ̃t���[���ɐݒ�
	_animTime = GetSecondFrame();

	//���A�j���[�V�����̍s����擾	
	BoneTransform(_animTime);

	//�O�A�j���[�V�����E���A�j���[�V�����̍s��̐��`��ԏ�������񏈗�
	auto f = [&]()
	{
		_isInBlend = true;

		float t = 0.0f;
		for (int i = 0; i <= 60 * BLEND_SPEED; i++)
		{
			_blendWeight = lerp(1.0f, 0.0f, t);

			for (int i = 0; i < _blendMats.size(); i++)
			{
				_blendMats[i] =
					(_beforeMats[i] * _blendWeight) + (_boneMats[i] * (1 - _blendWeight));
			}
			t += Dx12Wrapper::Instance().GetDeltaTime() / BLEND_SPEED;

			Sleep(1);
		}

		_isInBlend = false;
	};
	ParallelProcess(f);
}

/// <summary>
/// ����\���ǂ�����Ԃ��֐�
/// </summary>
/// <returns>�^���l</returns>
bool
FBXActor::GetCanControll()const
{
	return _canControll;
}

/// <summary>
/// �A�N�^�[�������Ă���A�j���[�V�����̖��O�̃x�N�g����Ԃ��֐�
/// </summary>
/// <returns>�A�j���[�V�������̃x�N�g��</returns>
vector<string>
FBXActor::GetAnimstr()const
{
	return _animStr;
}

/// <summary>
/// ���ݎ��s���Ă���A�j���[�V��������Ԃ��֐�
/// </summary>
/// <returns>�A�j���[�V������</returns>
string
FBXActor::GetCurentAnimStr()const
{
	return _currentActorAnim;
}

/// <summary>
/// �A�j���[�V�����̑����Ԃ�Ԃ��֐�
/// </summary>
/// <param name="animation">�A�j���[�V������</param>
/// <returns>������</returns>
float
FBXActor::GetAnimDuration(string animation)
{
	return static_cast<float>(_anims[CHARA_REF + animation]->mDuration);
}

/// <summary>
/// �A�j���[�V������1�b������̏����񐔂�Ԃ��֐�
/// </summary>
/// <returns>������</returns>
float
FBXActor::GetAnimTickPerSpeed(string animation)
{
	//�A�j���[�V������1�b������̏����񐔂��ݒ肳��Ă��Ȃ�������30��Ԃ�
	return static_cast<float>(_anims[CHARA_REF + animation]->mTicksPerSecond != 0
		? _anims[CHARA_REF + animation]->mTicksPerSecond : 30.0f);
}

/// <summary>
/// �A�j���[�V������2�ڂ̃t���[����Ԃ��֐�
/// </summary>
/// <returns>�t���[���̕b��</returns>
float
FBXActor::GetSecondFrame()
{
	return _secFrameTime;
}

/// <summary>
/// ���s����A�j���[�V�������w�肷��֐�
/// </summary>
/// <param name="animStr">�A�j���[�V������</param>
void
FBXActor::SetAnimStr(string animStr)
{
	_currentActorAnim = animStr;
}

/// <summary>
/// �A�j���[�V�����̍Đ����x��ݒ肷��
/// </summary>
/// <param name="speed">�Đ����x</param>
void 
FBXActor::SetAnimationSpeed(float speed)
{
	_animSpeed = speed;
}

/// <summary>
/// ImGui������A�j���[�V�����̍Đ����Ԃ�Ⴄ�֐�
/// </summary>
/// <param name="time">�Đ�����</param>
void
FBXActor::SetAnimationTime(float time)
{
	_animTime = time + GetSecondFrame();
}

/// <summary>
/// �A�j���[�V������ύX�\�����߂�֐�
/// </summary>
/// <param name="val">�^���l</param>
void
FBXActor::SetCanChangeAnim(bool val)
{
	_canChangeAnim = val;
}

/// <summary>
/// �A�j���[�V���������[�v���邩���߂�֐�
/// </summary>
/// <param name="val">�^���l</param>
void
FBXActor::SetIsInLoop(bool val)
{
	_isInLoop = val;
}

/// <summary>
/// �ڒn��������߂铖���蔻���Ԃ�
/// </summary>
/// <returns>�����蔻��</returns>
shared_ptr<BoxCollider> 
FBXActor::GetColForGround()const
{
	return _colForGround;
}

/// <summary>
/// �n�ʂɐڂ��Ă��邩�ǂ�����Ԃ�
/// </summary>
/// <returns>�^���l</returns>
bool
FBXActor::GetOnGround()const
{
	return _isOnGround(FBXBase::_pos);
}

/// <summary>
/// ���݂̃A�j���[�V�������擾����֐�
/// </summary>
/// <param name="anim">��r�������A�j���[�V����</param>
/// <returns>�����ƌ��݂̃A�j���[�V��������������</returns>
bool
FBXActor::IsAnimationEqual(AnimEnum anim)const
{
	if (!_crntNode)
	{
		return false;
	}
	else
	{
		return _crntNode->GetAnimEnum() == anim;
	}
}