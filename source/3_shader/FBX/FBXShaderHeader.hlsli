//FBX���f���p�w�b�_�[�t�@�C��
//�V�F�[�_�[�ŗp����e��f�[�^���܂Ƃ߂Ă���

Texture2D<float4> tex : register(t0);	//�e�N�X�`��
SamplerState smp : register(s0);		//�T���v���[�i�e�N�X�`���̎擾���@�����߂�j

//�r���[�v���W�F�N�V�����p�X���b�g
cbuffer SceneData : register(b0)
{
	matrix view;						//�r���[�s��
	matrix proj;						//�v���W�F�N�V�����s��
	float3 eye;							//�J�������W
};

//���W�ϊ��p�X���b�g
cbuffer Transform : register(b1)
{
	matrix world;						//���[���h�ϊ��s��
	matrix bone[256];					//�{�[�����̕ϊ��s��
}

//�p�C�v���C���X�e�[�g���痬��Ă���f�[�^���󂯎~�߂�\����
struct Output
{
	float4 svpos	: SV_POSITION;		//�V�X�e���p���W
	float4 pos		: POSITION;			//���W
	float4 normal	: NORMAL;			//�@��
	float2 uv		: TEXCOORD;			//�e�N�X�`����UV�l
	float3 tangent	: TANGENT;			//����
	float4 color	: COLOR;			//�J���[
	uint4  ids		: IDs;				//���_�ɉe����^����{�[����ID�i���_1������4�{�j
	float4 weight	: WEIGHT;			//�e�{�[���̃E�F�C�g�i���_1������4�{�j
};