#include"FBXShaderHeader.hlsli"

//FBX���f���p�s�N�Z���V�F�[�_�[
//UV���W����ɒ��_�Ƀe�N�X�`����t�^����
float4 FBXPS(Output input) : SV_TARGET
{
	float4 ret = tex.Sample(smp,input.uv);

	float len = length(input.dis);
	float disFromEye = (len - 950.0f);
	
	ret.a = (disFromEye < 0.0f ? 1.0f : 1.0f - (disFromEye/ 50.0f));

	return ret;
}