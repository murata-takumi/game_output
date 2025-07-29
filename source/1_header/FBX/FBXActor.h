#pragma once
#include "FBX/AnimNodes/Animations.h"
#include "FBX/IFbx.h"

/// <summary>
/// �Q�[����ʏ�ɓo�ꂷ��3D�I�u�W�F�N�g�̃N���X�i�{�[������j
/// </summary>
class aiAnimation;
class aiNode;
class aiNodeAnim;
class AnimNode;
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class FbxComposition;
class ICollider;
class ImGuiManager;
class Vector3;
class FbxActor : public IFbx
{	
	template<typename T>using ComPtr = ComPtr<T>;

private:
	//���n��������o���邽�߂̓����蔻��
	shared_ptr<ICollider> _colForGround;

	//�A�j���[�V�������s�p�m�[�h�̔z��
	unique_ptr<AnimNode> _animNodes[Length];
	//���ݎ��s����m�[�h
	AnimNode* _crntNode = nullptr;

	//FBX�N���X�̋��ʏ������܂Ƃ߂��C���X�^���X
	shared_ptr<FbxComposition> _fbxComp;

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
	Vector3 _currentFrontVec = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

	//���̓x�N�g��
	Vector3 _inputVec;

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
	function<bool(const Vector3& vec)> _isOnGround;

	//�R���X�g���N�^
	FbxActor() = default;
	//�f�X�g���N�^
	~FbxActor() = default;

	HRESULT Init(const wchar_t* filePath, const string name,
		const Vector3& size, const Vector3& pos, ColliderType colType)override;

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

	//�`�揈��
	void Draw()override;

	//���t���[���̍��W�ϊ�����
	void Update()override;

	//�L�[�����͂��ꂽ���ɌĂяo���֐�
	void OnKeyPressed(const Vector3& input);

	void AdjustPos(const Vector3& diff);

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

	//���݂̍��W���擾
	Vector3 CurrentPosition()override;

	//�{�̂̓����蔻���Ԃ�
	shared_ptr<ICollider>  Collider();

	//�ڒn������s��OBB��Ԃ�
	shared_ptr<ICollider> GetColForGround()const;

	//�n�ʂɐڂ��Ă��邩�ǂ�����Ԃ�
	bool GetOnGround()const;
};