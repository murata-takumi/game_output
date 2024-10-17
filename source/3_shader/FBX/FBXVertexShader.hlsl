#include"FBXShaderHeader.hlsli"

//FBX���f���p���_�V�F�[�_�[
//�p�C�v���C����ʂ��Ď󂯎�������_�ɑ΂��e�s��ō��W�ϊ����s��
Output FBXVS
(
	float4 pos : POSITION,
	float4 normal : NORMAL,
	float2 uv : TEXCOORD,
	float3 tangent : TANGENT,
	float4 color : COLOR,
	uint4 ids : IDs,
	float4 weight : WEIGHT
)
{
	//�P�ʍs��
	const matrix identity =												
		float4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			);
	//�[���s��
	const matrix zeroMat =												
		float4x4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
			);

	//�o�͗p�\����
	Output output;													

	//���_�ɉe����^����4�̃{�[���s����E�F�C�g�ɉ����ĉ��Z
	matrix boneTransform = bone[ids.r] * weight.r;					
	boneTransform += bone[ids.g] * weight.g;
	boneTransform += bone[ids.b] * weight.b;
	boneTransform += bone[ids.a] * weight.a;

	//�{�[���s�񂪉��Z����Ă��Ȃ�������P�ʍs�����
	boneTransform =													
		(boneTransform == zeroMat) ? identity : boneTransform;

	//���W�ϊ�
	//�{�[���s��𔽉f
	float4 localPos = mul(boneTransform, float4(pos));				
	//���[���h�s��𔽉f
	float4 worldPos = mul(world, localPos);							
	//�r���[�s��𔽉f
	float4 viewPos = mul(view, worldPos);							
	//�v���W�F�N�V�����s��𔽉f
	float4 projPos = mul(proj, viewPos);
	
	//���_���W
    output.pos = projPos;
	//�V�X�e�����W
	output.svpos = projPos;	
    output.normal = normal;
	//�J���[
	output.color = color;											
	//UV���W
	output.uv = uv;

	return output;
}