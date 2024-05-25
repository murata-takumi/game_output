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
	const matrix identity =												//�P�ʍs��
		float4x4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
			);
	const matrix zeroMat =												//�[���s��
		float4x4(
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 0, 0
			);

	Output output;													//�o�͗p�\����

	matrix boneTransform = bone[ids.r] * weight.r;					//���_�ɉe����^����4�̃{�[���s����E�F�C�g�ɉ����ĉ��Z
	boneTransform += bone[ids.g] * weight.g;
	boneTransform += bone[ids.b] * weight.b;
	boneTransform += bone[ids.a] * weight.a;

	boneTransform =													//�{�[���s�񂪉��Z����Ă��Ȃ�������P�ʍs�����
		(boneTransform == zeroMat) ? identity : boneTransform;

	//���W�ϊ�
	float4 localPos = mul(boneTransform, float4(pos));				//�{�[���s��𔽉f
	float4 worldPos = mul(world, localPos);							//���[���h�s��𔽉f
	float4 viewPos = mul(view, worldPos);							//�r���[�s��𔽉f
	float4 projPos = mul(proj, viewPos);							//�v���W�F�N�V�����s��𔽉f

	output.pos = viewPos;											//���_���W
	output.svpos = projPos;											//���_���W
	output.color = color;											//�J���[
	output.uv = uv;													//UV���W

	return output;
}