#include"peraHeader.hlsli"

//��ʗp���_�V�F�[�_�[
//���_���W�AUV���W��n���Ă���
Output PeraVS(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Output output;			//�o�͗p�\���̂�錾
	output.svpos = pos;		//���_���W�����
	output.uv = uv;			//UV���W�����

	return output;			//�s�N�Z���V�F�[�_�[�ɓn��
}