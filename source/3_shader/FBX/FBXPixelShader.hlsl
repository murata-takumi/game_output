#include"FBXShaderHeader.hlsli"

//FBX���f���p�s�N�Z���V�F�[�_�[
//UV���W����ɒ��_�Ƀe�N�X�`����t�^����
float4 FBXPS(Output input) : SV_TARGET
{
	float4 ret = tex.Sample(smp,input.uv);

	float len = length(input.dis);
	
	ret.a = 0.0f;

	return ret;
}