//FBX���f���p�w�b�_�[�t�@�C��
//�V�F�[�_�[�ŗp����e��f�[�^���܂Ƃ߂Ă���

//�e�N�X�`��
Texture2D<float4> tex : register(t0);	
//�g�D�[���e�N�X�`��
Texture2D<float4> toonTex : register(t1);	

//�T���v���[�i�e�N�X�`���̎擾���@�����߂�j
SamplerState smp : register(s0);	
//�T���v���[�i�g�D�[���e�N�X�`���̎擾���@�����߂�j
SamplerState toonSmp : register(s1);

//�r���[�v���W�F�N�V�����p�X���b�g
cbuffer SceneData : register(b0)
{
	//�r���[�s��
	matrix view;						
	//�v���W�F�N�V�����s��
	matrix proj;						
	//�J�������W
	float3 eye;							
};

//���W�ϊ��p�X���b�g
cbuffer Transform : register(b1)
{
	//���[���h�ϊ��s��
	matrix world;						
	//�{�[�����̕ϊ��s��
	matrix bone[256];					
}

//�p�C�v���C���X�e�[�g���痬��Ă���f�[�^���󂯎~�߂�\����
struct Output
{
	//�V�X�e���p���W
	float4 svpos	: SV_POSITION;		
	//���W
	float4 pos		: POSITION;			
	//�@��
	float4 normal	: NORMAL;			
	//�e�N�X�`����UV�l
	float2 uv		: TEXCOORD;
	//����	
	float3 tangent	: TANGENT;
	//�J���[		
	float4 color	: COLOR;
	//���_�ɉe����^����{�[����ID�i���_1������4�{�j
	uint4  ids		: IDs;			
	//���_�ɉe����^����{�[���̏d�݁i���_1������4�{�j
	float4 weight	: WEIGHT;			
	//�J�������W����̋����i�ڂ₯�鏈���Ɏg�p�j
	float3 dis		: DISFROMEYE;
};