//��ʗp�V�F�[�_�[�̃w�b�_�[�t�@�C��
//�����ł�����ʂƂ́A���f�����̃����_�����O���ʂ̂��Ƃł���
//�V�F�[�_�[�Ŏg�p����f�[�^���܂Ƃ߂Ă���

Texture2D<float4> tex : register(t0);		//��ʃe�N�X�`��
SamplerState smp : register(s0);			//�T���v���[

//�t�F�[�h�C���E�A�E�g�����肷��X���b�g
cbuffer Fade : register(b0)
{
	float fade;
};				

//�o�͗p�\����
struct Output
{
	float4 svpos : SV_POSITION;				//���_���
	float2 uv : TEXCOORD;					//UV���W
};