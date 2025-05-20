#pragma once
#include "Collider/CollisionDetector.h"
#include "FBX/AnimNodes/Animations.h"
#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/FBXBase.h"

/// <summary>
/// �Q�[����ʏ�ɓo�ꂷ��3D�I�u�W�F�N�g�̃N���X�i�{�[������j
/// </summary>
class Dx12Wrapper;
class AssimpLoader;
class AnimNode;
class FBXActor : public FBXBase
{	
	template<typename T>using ComPtr = ComPtr<T>;

private:

	//�A�j���[�V�������s�p�m�[�h�̔z��
	unique_ptr<AnimNode> _animNodes[Length];
	//���ݎ��s����m�[�h
	AnimNode* _crntNode = nullptr;												

	//�{�[���s��̃x�N�g��
	vector<XMMATRIX> _boneMats;	
	//�u�����h���̑J�ڑO���[�V�����̍s��
	vector<XMMATRIX> _beforeMats;
	//�A�N�^�[�̍��W�n���烏�[���h���W�n�ɖ߂��]�u�s��
	vector<XMMATRIX> _blendMats;			
	vector<XMMATRIX> _invMats;	

	//�A�j���[�V�����Ƃ��̖��O�̘A�z�z��
	map<string, aiAnimation*> _anims;										

	//���ݍĐ����Ă���A�j���[�V������
	string _currentActorAnim;		
	//�A�j���[�V�������̃x�N�g��
	vector<string> _animStr;												

	//���ʃx�N�g��
	XMVECTOR _currentFrontVec = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

	//���̓x�N�g��
	XMVECTOR _inputVec;

	//����\���𔻕ʂ���^���l
	bool _canControll = false;			
	//�u�����h�����𔻕ʂ���^���l
	bool _isInBlend = false;												

	//�A�j���[�V���������[�v�����邩���߂�^���l
	bool _isInLoop = true;	
	//�A�j���[�V�������ύX�\�����߂�^���l
	bool _canChangeAnim = true;													

	//�ŏI�I�ȃA�N�^�[�̉�]�p�iY���j
	float _destRad = 0.0f;
	//���[�V�����u�����h���Ɏg�p����E�F�C�g�l
	float _blendWeight = 0.0f;	
	//�A�j���[�V�����̍Đ����x
	float _animSpeed = 1.0f;
	//�A�j���[�V�����̍Đ�����
	float _animTime = 0.0f;			
	//�A�j���[�V�����̑�2�t���[���iT�|�[�Y�łȂ��|�[�Y�̍ŏ��̃t���[���j�̕b��
	float _secFrameTime;			
	//�A�N�^�[�̌����iY���j
	float _rotY = 0.0f;
	//Z������v���C���[�̌����܂ł̊p�x
	float _zToFrontAngle;

	//�O�t���[���̎���
	LARGE_INTEGER _befFrameTime;									
	//���t���[���̎���
	LARGE_INTEGER _currFrameTime;											

	//�m�[�h�K�w��ǂݍ��ފ֐�
	void ReadNodeHeirarchy(float animationTime,const aiNode* pNode, const XMMATRIX& parentTrans);

	//aiAnimation����m�[�h������v����aiNodeAnim�����o���֐�
	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);

	//�X�P�[�����O�A�j���[�V������⊮����֐�
	XMMATRIX CalcInterpolatedScaling(float animationTime,const aiNodeAnim* nodeAnim);
	//��]�A�j���[�V������⊮����֐�
	XMMATRIX CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim);
	//���W�ړ��A�j���[�V������⊮����֐�
	XMMATRIX CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim);

	//�X�P�[�����O�A�j���[�V�����̃L�[�����o���֐�
	int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	//��]�A�j���[�V�����̃L�[�����o���֐�
	int FindRotation(float animationTime,const aiNodeAnim* nodeAnim);		
	//���W�ړ��A�j���[�V�����̃L�[�����o���֐�
	int FindPosition(float animationTime,const aiNodeAnim* nodeAnim);		

	//�A�j���[�V�����֘A�̏��������s���֐�
	void InitAnimation();													

	//���f���̃A�j���[�V�����p�s������߂�֐�
	void BoneTransform(float timeInTicks);									

	//���W�ϊ��p�o�b�t�@�[�E�r���[�쐬�֐�
	HRESULT CreateTransformView()override;									
public:	
	//�A�N�^�[���n�ʂ̏�ɂ��邩���ʂ��邽�߂̃R�[���o�b�N
	function<bool(const XMVECTOR& vec)> _isOnGround;

	//�R���X�g���N�^
	FBXActor(const wchar_t* filePath,const string name, XMFLOAT3 size, XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f));
	//�f�X�g���N�^
	~FBXActor();														

	//�A�j���[�V�������̃x�N�g����Ԃ��֐�
	vector<string> GetAnimstr()const;
	//���ݎ��s���Ă���A�j���[�V��������Ԃ��֐�
	string GetCurentAnimStr()const;

	//�A�j���[�V�����̑����Ԃ�Ԃ��֐�
	float GetAnimDuration(string animation);	
	//�A�j���[�V������1�b������̏����񐔂�Ԃ��֐�
	float GetAnimTickPerSpeed(string animation);
	//�A�j���[�V������2�ڂ̃t���[����Ԃ��֐�
	float GetSecondFrame();												

	//���s����A�j���[�V���������w�肷��֐�
	void SetAnimStr(string animStr);
	//�A�j���[�V�����̍Đ����x��ݒ肷��֐�
	void SetAnimationSpeed(float speed);
	//�A�j���[�V�����̍Đ����Ԃ�ݒ肷��֐�
	void SetAnimationTime(float time);					
	//�A�N�^�[��T�|�[�Y�ɂ���֐�
	void InitPose();													

	//���t���[���̍��W�ϊ�����
	void Update()override;												

	//���͂��ꂽ�����ֈړ�����֐�
	void Translate(const XMVECTOR& vec);								

	//����J�n���Ɏ��s����֐�
	void StartControll();				
	//�������֐�
	void EndControll();													
	//����\���ǂ�����Ԃ��֐�
	bool GetCanControll()const;											

	//�A�j���[�V�����������ɓ��������Ԃ��֐�
	bool IsAnimationEqual(AnimEnum anim)const;								
	//�A�j���[�V�����Ԃ̃u�����h�����s����֐�
	void SetAnimationNode(AnimEnum anim);		
	//�A�j���[�V�������Z�b�g����֐�
	void BlendAnimation(AnimEnum anim);									

	//�A�j���[�V�������s�\���ݒ肷��
	void SetCanChangeAnim(bool val);
	//�A�j���[�V�������[�v�\���ݒ肷��
	void SetIsInLoop(bool val);
	//�n�ʂɐڂ��Ă��邩�ǂ�����Ԃ�
	bool GetOnGround()const;
};