#include"FBXShaderHeader.hlsli"

//FBX���f���p�s�N�Z���V�F�[�_�[
//UV���W����ɒ��_�Ƀe�N�X�`����t�^����
float4 FBXPS(Output input) : SV_TARGET
{
	return tex.Sample(smp,input.uv);
}