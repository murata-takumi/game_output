#include"peraHeader.hlsli"

//��ʗp�s�N�Z���V�F�[�_�[
//��ʂɃt�F�[�h�C���E�A�E�g���̉��o��t�^����
float4 PeraPS(Output input) : SV_TARGET
{
	float4 res = tex.Sample(smp, input.uv);		//��ʂ�`��
	res.rgb -= (res.rgb * fade);				//��ʃe�N�X�`����RGB�𒲐����A�t�F�[�h�C���E�A�E�g����������

	return res;
}