#include "Functions.h"
#include "FBX/FBXActor.h"
#include "Wrapper/Dx12Wrapper.h"

//アニメーション名に付与する文字列リテラル
const char* CHARA_REF = "Character1_Reference|";
//移動速度
const float MOVE_SPEED = 300.0f;
//重力加速度
const float GRAVITY_ACCERALATION = 9.8f;
//アニメーションのブレンドに掛ける秒数
const float BLEND_SPEED = 0.2f;
//アニメーション名の不要な文字列を削除するためのインデックス
const int ANIM_STR_UNNECESSARY_IDX = 21;
//当たり判定を付与するボーンのインデックス
const int COLLIDER_BONE = 0;								

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="dx12">Dx12Wrapperインスタンス</param>
/// <param name="filePath">モデルのパス</param>
FBXActor::FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath, XMFLOAT3 size, XMFLOAT3 pos)
	:FBXBase(dx12,filePath, size, pos),
	_crntNode(nullptr), _frontVec(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
	_canControll(false),_isInBlend(false),_isInLoop(true),_isOnGround(true),
	_canChangeAnim(true),_blendWeight(0.0f), _animTime(0.0f),
	_destRad(0.0f), _rotY(0.0f)
{
	//アニメーションノードを初期化
	_animNodes[WAIT00] = make_unique<WAIT00Node>(this,WAIT00);
	_animNodes[RUN00_F] = make_unique<RUN00_FNode>(this, RUN00_F);
	_animNodes[JUMP00] = make_unique<JUMP00Node>(this, JUMP00);
	_animNodes[FALL] = make_unique<FallNode>(this, FALL);

	//アニメーションに関するデータを初期化
	InitAnimation();															

	//座標変換用バッファー・ビュー作成
	CreateTransformView();														
}

/// <summary>
/// デストラクタ
/// ポインタを開放する
/// </summary>
FBXActor::~FBXActor()
{

}

/// <summary>
/// アニメーション関連の初期化を行う関数
/// </summary>
void
FBXActor::InitAnimation()
{
	//ボーン行列の個数を決める
	_beforeMats.resize(_boneMats.size());
	_boneMats.resize(_boneInfo.size());		
	//ブレンド用の行列の個数も決める
	_blendMats.resize(_boneInfo.size());							
	//逆変換用の行列の個数も決める
	_invMats.resize(_boneInfo.size());

	//各ボーン行列のスケーリング・平行移動を無効化した行列を作成
	for (UINT i = 0; i < _invMats.size(); i++)
	{
		//ボーン行列を分解し、クォータニオンをX軸に-90度、Y軸に90度回転
		XMVECTOR scale, skew, trans;
		XMMatrixDecompose(&scale, &skew, &trans, 
			XMMatrixInverse(nullptr, _boneInfo[i]._boneOffset));

		auto q1 = XMQuaternionRotationAxis(							
			XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XM_PI / -2);
		auto q2 = XMQuaternionRotationAxis(
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XM_PI / 2);
		skew = XMQuaternionMultiply(skew, q1);
		skew = XMQuaternionMultiply(skew, q2);

		//行列を再合成
		XMMATRIX invMat = XMMatrixScalingFromVector(scale) *		
						XMMatrixRotationQuaternion(skew) * 
						XMMatrixTranslationFromVector(trans);

		//行列ベクトルに格納
		_invMats[i] = invMat;										
	}

	//モデルが持つ全アニメーションに対し実行
	for (UINT i = 0; i < _scene->mNumAnimations; i++)				
	{
		//連想配列にアニメーション名とアニメーションデータを格納
		_anims[_scene->mAnimations[i]->mName.C_Str()]				
			= _scene->mAnimations[i];

		//アニメーション名を取得
		string name = _scene->mAnimations[i]->mName.C_Str();		

		//余分な文字列を削除
		name = name.erase(0, ANIM_STR_UNNECESSARY_IDX);									

		//アニメーション名のベクトルに追加
		_animStr.push_back(name);									
	}

	//最初に実行するアニメーションを設定
	_currentActorAnim = _animStr[0];								

	//Tポーズにする
	InitPose();														
}

/// <summary>
/// ボーン変換行列を書き込む関数
/// </summary>
/// <param name="timeInTicks">現在の経過時間</param>
void
FBXActor::BoneTransform(float timeInTicks)
{
	//経過時間を基に階層構造から変換行列を読み込む
	if (timeInTicks < GetAnimDuration(_currentActorAnim))									
	{
		ReadNodeHeirarchy(timeInTicks, _scene->mRootNode, XMMatrixIdentity());
	}

	//変換行列を更新する
	for (size_t i = 0,boneLength = _boneMats.size(); i < boneLength; ++i)					
	{
		_boneMats[i] = _boneInfo[i]._finalTrans;
	}
}

/// <summary>
/// アニメーションノードを設定する関数
/// </summary>
/// <param name="anim">呼び出したいアニメーション</param>
void
FBXActor::SetAnimationNode(AnimEnum anim)
{
	//アニメーションを変更できる状態か確認
	//同じアニメーションの再生しようとした時も関数を終了させる
	if (!_canChangeAnim || IsAnimationEqual(anim)) return;

	//前ノードの終了処理
	if (_crntNode != nullptr)
	{
		_crntNode->EndAnimation();		
	}

	//ノード更新
	_crntNode = _animNodes[anim].get();
	//現ノードの開始処理
	_crntNode->StartAnimation();
}

/// <summary>
/// 再帰的にノードの階層を読み込み、座標変換を行う行列を取得する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="pNode">親ノード</param>
/// <param name="parentTrans">親ノードで適用されている行列</param>
void
FBXActor::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const XMMATRIX& parentTrans)
{
	//ノード名
	string nodeName(pNode->mName.data);													
	//アニメーションを取得
	const aiAnimation* pAnimation = _anims[CHARA_REF + _currentActorAnim];				
	//ノードのaiMatrix4x4をXMMATRIXに変換
	aiMatrix4x4 aiTrans = pNode->mTransformation;										
	XMFLOAT4X4 temp = XMFLOAT4X4
	(
		aiTrans.a1, aiTrans.b1, aiTrans.c1, aiTrans.d1,
		aiTrans.a2, aiTrans.b2, aiTrans.c2, aiTrans.d2,
		aiTrans.a3, aiTrans.b3, aiTrans.c3, aiTrans.d3,
		aiTrans.a4, aiTrans.b4, aiTrans.c4, aiTrans.d4
	);
	XMMATRIX nodeTrans = XMLoadFloat4x4(&temp);
	//ノード名と同名のaiNodeAnimを取り出す
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, nodeName);					

	//アニメーションがある場合各行列を取得する
	if (pNodeAnim != nullptr)															
	{
		//スケーリング
		XMMATRIX scalingM = CalcInterpolatedScaling(animationTime, pNodeAnim);
		//回転
		XMMATRIX rotationM = CalcInterpolatedRotation(animationTime, pNodeAnim);		
		//平行移動
		XMMATRIX translationM = CalcInterpolatedPosition(animationTime, pNodeAnim);		
		//行列を合成
		nodeTrans = scalingM * rotationM * translationM;								
	}

	//親の変換行列を適用
	XMMATRIX globalTrans = nodeTrans * parentTrans;										

	//ボーン名に対応するインデックスを取得し、それを基に最終的な変換行列を取得
	if (_boneMapping.find(nodeName) != _boneMapping.end())
	{
		unsigned int boneIdx = _boneMapping[nodeName];

		_boneInfo[boneIdx]._finalTrans = _boneInfo[boneIdx]._boneOffset * globalTrans;
	}

	//子ノードの方にも再帰処理を行う
	auto numChildren = pNode->mNumChildren;
	for (UINT i = 0; i < numChildren; i++)												
	{
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], globalTrans);
	}
}

/// <summary>
/// オブジェクトの座標変換に用いられるヒープ・ビューを作成する関数
/// </summary>
/// <returns>作成できたかどうか</returns>
HRESULT
FBXActor::CreateTransformView()
{
	result = S_OK;

	//ワールド行列用バッファーの作成
	auto buffSize = sizeof(XMMATRIX) * (1 + _boneMats.size());						
	buffSize = (buffSize + 0xff) & ~0xff;
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(buffSize);
	result = _dx12.Device()->CreateCommittedResource
	(
		&_uploadHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(_transBuffer.ReleaseAndGetAddressOf())
	);
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//座標変換用行列の書き込み、初期座標に位置するようにする
	result = _transBuffer->Map(0, nullptr, (void**)&_mappedMats);					
	if (FAILED(result))
	{
		assert(0);
		return result;
	}
	_mappedMats[0] = XMMatrixIdentity();
	_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);
	copy(_boneMats.begin(), _boneMats.end(), _mappedMats + 1);

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC transformDescHeapDesc = {};							
	transformDescHeapDesc.NumDescriptors = 1;
	transformDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	transformDescHeapDesc.NodeMask = 0;
	transformDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	result = _dx12.Device()->CreateDescriptorHeap(&transformDescHeapDesc,
		IID_PPV_ARGS(_transHeap.ReleaseAndGetAddressOf()));
	if (FAILED(result)) {
		assert(0);
		return result;
	}

	//ビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
	cbvDesc.BufferLocation = _transBuffer->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = static_cast<UINT>(_transBuffer->GetDesc().Width);
	_dx12.Device()->CreateConstantBufferView(&cbvDesc,								
		_transHeap->GetCPUDescriptorHandleForHeapStart());

	//当たり判定を初期化
	_collider->Update(_mappedMats[0]);												

	return result;
}

/// <summary>
/// アニメーション配列の中から名前が一致したアニメーションを取り出す関数
/// </summary>
/// <param name="animation">アニメーション配列</param>
/// <param name="nodeName">アニメーション</param>
/// <returns>ノード名が一致したアニメーション</returns>
const aiNodeAnim*
FBXActor::FindNodeAnim(const aiAnimation* animation, const string nodeName)
{
	for (UINT i = 0; i < animation->mNumChannels; i++)
	{
		//aiNodeAnimの名前を取得し引数のアニメーション名と一致していたらそれを返す
		const aiNodeAnim* nodeAnim = animation->mChannels[i];	
		if (string(nodeAnim->mNodeName.data) == nodeName)
		{
			return nodeAnim;
		}
	}

	//なかったらnullptr
	return nullptr;												
}

/// <summary>
/// スケーリングの補間を実施する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>スケーリング行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	//スケーリングで用いるキーが1つの場合、そのキーのベクトルをそのまま取得
	aiVector3D tempVec;
	if (nodeAnim->mNumScalingKeys == 1)											
	{
		tempVec = nodeAnim->mScalingKeys[0].mValue;
	}
	//そうでない場合、現フレームと次フレームからベクトルを求める
	else
	{
		//経過時間を基にキーのインデックスと次のキーのインデックスを取得
		int scalingIdx = FindScaling(animationTime, nodeAnim);
		UINT nextScalingIdx = scalingIdx + 1;
		assert(nextScalingIdx < nodeAnim->mNumScalingKeys);

		//キー同士の経過時間の差分を取得
		float deltaTime = static_cast<float>(
			nodeAnim->mScalingKeys[nextScalingIdx].mTime
			- nodeAnim->mScalingKeys[scalingIdx].mTime);

		//実際の経過時間のキー同士の差分に対する倍率を求める
		float factor =
			(animationTime
				- static_cast<float>(nodeAnim->mScalingKeys[scalingIdx].mTime)
				)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//現フレーム、次フレームのベクトルの差分を求める
		const aiVector3D& start = nodeAnim->mScalingKeys[scalingIdx].mValue;
		const aiVector3D& end = nodeAnim->mScalingKeys[nextScalingIdx].mValue;
		aiVector3D delta = end - start;

		//ベクトルの差分に倍率をかけ、それを現フレームのベクトルに加算
		tempVec = start + factor * delta;
	}

	////aiVector3DからXMVectorへ変換し、更にXMMatrixに変換
	XMVECTOR scaling(XMVectorSet(tempVec.x, tempVec.y, tempVec.z, 0));
	XMMATRIX ret = XMMatrixScalingFromVector(scaling);

	return ret;
}

/// <summary>
/// 回転の補完を実行する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>回転行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiQuaternion tempQuat;

	//キーが一つしかない場合そのキーのクォータニオンを取得
	if (nodeAnim->mNumRotationKeys == 1)														
	{
		tempQuat = nodeAnim->mRotationKeys[0].mValue;
	}
	else
	{
		//経過時間を基にキーと次のキーのインデックスを取得
		int rotationIdx = FindRotation(animationTime, nodeAnim);								
		UINT nextRotationIdx = rotationIdx + 1;
		assert(nextRotationIdx < nodeAnim->mNumRotationKeys);

		//2つのキーの時間の差分を取得
		float deltaTime = static_cast<float>													
			(
				nodeAnim->mRotationKeys[nextRotationIdx].mTime -
				nodeAnim->mRotationKeys[rotationIdx].mTime
				);

		//キーの時間の差分に対する経過時間から次のキーの時間の割合を取得
		float factor =																			
			(animationTime - static_cast<float>(nodeAnim->mRotationKeys[rotationIdx].mTime))
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//キーと次のキーのクォータニオン
		const aiQuaternion& start = nodeAnim->mRotationKeys[rotationIdx].mValue;				
		const aiQuaternion& end = nodeAnim->mRotationKeys[nextRotationIdx].mValue;

		//補間して正規化
		aiQuaternion::Interpolate(tempQuat, start, end, factor);								
		tempQuat = tempQuat.Normalize();
	}

	//クォータニオンからaiMatrix3x3を取得、さらにXMMatrixに変換
	aiMatrix3x3 rotMat = tempQuat.GetMatrix();													
	auto rotTemp = XMFLOAT3X3
	(
		rotMat.a1, rotMat.b1, rotMat.c1,
		rotMat.a2, rotMat.b2, rotMat.c2,
		rotMat.a3, rotMat.b3, rotMat.c3
	);
	XMMATRIX ret = XMLoadFloat3x3(&rotTemp);

	return ret;
}

/// <summary>
/// 平行移動の補間を実行する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーション</param>
/// <returns>平行移動行列</returns>
XMMATRIX
FBXActor::CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	aiVector3D tempPos;

	//キーが一つだけだったら対応する移動ベクトルをそのまま取得
	if (nodeAnim->mNumPositionKeys == 1)											
	{
		tempPos = nodeAnim->mPositionKeys[0].mValue;
	}
	else
	{
		//経過時間を基にキーのインデックスを取得
		int positionIdx = FindPosition(animationTime, nodeAnim);					
		UINT nextPositionIdx = positionIdx + 1;
		assert(nextPositionIdx < nodeAnim->mNumPositionKeys);

		//二つのキーの時間の差分を取得
		float deltaTime =															
			static_cast<float>(
				nodeAnim->mPositionKeys[nextPositionIdx].mTime - 
				nodeAnim->mPositionKeys[positionIdx].mTime
			);
		//キーの時間の差分に対する経過時間から次のキーの差分の割合を求める
		float factor =																
			(
				animationTime - 
				static_cast<float>(nodeAnim->mPositionKeys[positionIdx].mTime)
			)
			/ deltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		//二つのキーに対応する移動ベクトルの差分を取得し、割合を用いて補完する
		const aiVector3D& start = nodeAnim->mPositionKeys[positionIdx].mValue;
		const aiVector3D& end = nodeAnim->mPositionKeys[nextPositionIdx].mValue;
		aiVector3D delta = end - start;
		tempPos = start + factor * delta;
	}
	//aiVector3DからXMVectorに変換し、その後XMMatrixに変換
	XMVECTOR translation(XMVectorSet(tempPos.x, tempPos.y, tempPos.z, 0));			
	XMMATRIX ret = XMMatrixTranslationFromVector(translation);

	return ret;
}

/// <summary>
/// スケーリングアニメーションのキーのインデックスを取得する関数
/// </summary>
/// <param name="animationTime">現在の経過時間</param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int
FBXActor::FindScaling(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numSclKeys = nodeAnim->mNumScalingKeys;
	assert(numSclKeys > 0);

	//ループを回し、経過時間を上回るキーを返す
	for (UINT i = 0; i < numSclKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mScalingKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// 回転に関するアニメーションデータのインデックスを取得する関数
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int 
FBXActor::FindRotation(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numRotKeys = nodeAnim->mNumRotationKeys;
	assert(numRotKeys > 0);

	//ループを回し、経過時間を上回るキーを返す
	for (UINT i = 0; i < numRotKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mRotationKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// 平行移動に関するアニメーションデータのインデックスを取得する関数
/// </summary>
/// <param name="animationTime"></param>
/// <param name="nodeAnim">アニメーションデータ</param>
/// <returns>インデックス</returns>
int
FBXActor::FindPosition(float animationTime, const aiNodeAnim* nodeAnim)
{
	auto numPosKeys = nodeAnim->mNumPositionKeys;
	assert(numPosKeys > 0);

	//ループを回し、経過時間を上回るキーを返す
	for (UINT i = 0; i < numPosKeys - 1; ++i)
	{
		if (animationTime < static_cast<float>(nodeAnim->mPositionKeys[i + 1].mTime))
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

/// <summary>
/// アクターをTポーズにする関数
/// </summary>
void
FBXActor::InitPose()
{
	_secFrameTime = _anims[CHARA_REF + _currentActorAnim]->mChannels[0]->mPositionKeys[1].mTime;
}

/// <summary>
/// 毎フレームの座標変換処理を実行する関数
/// </summary>
void
FBXActor::Update()
{
	//操作可能な場合
	if (_canControll)			
	{
		//現フレームの時間を取得し前フレームの時間との差分を経過時間に加算
		QueryPerformanceCounter(&_currFrameTime);
		auto diff = GetLIntDiff(_currFrameTime, _befFrameTime);
		_animTime += static_cast<float>(diff) * GetAnimTickPerSpeed(_currentActorAnim) * _animSpeed;

		//最初のフレームを無視したうえでアニメーションがループするよう設定
		if (_isInLoop)
		{
			_animTime = fmod(_animTime, GetAnimDuration(_currentActorAnim));		
			_animTime = clamp(_animTime,GetSecondFrame(), GetAnimDuration(_currentActorAnim));
		}

		//アニメーションノードの更新
		_crntNode->Update(_animTime);												

		if (!_isOnGround && !IsAnimationEqual(JUMP00))_pos.m128_f32[1] -= _dx12.GetDeltaTime() * 45.0f * GRAVITY_ACCERALATION;

		//回転、平行移動
		_mappedMats[0] = XMMatrixRotationY(_rotY);									
		_mappedMats[0] *= XMMatrixTranslationFromVector(_pos);
	}

	//スケーリング、回転、平行移動成分を変換行列とボーン行列の積から取得し、回転要素を削除したうえで再度合成
	XMVECTOR scale, trans, skew;													
	XMMatrixDecompose(&scale, &skew, &trans, 
		_invMats[COLLIDER_BONE] * _mappedMats[COLLIDER_BONE + 1]);
	skew = XMVectorZero();
	XMMATRIX newWorldMat = XMMatrixScalingFromVector(scale) * XMMatrixRotationQuaternion(skew) * XMMatrixTranslationFromVector(trans);

	//当たり判定を更新
	_collider->Update(newWorldMat *	_mappedMats[0]);

	//経過時間を渡し、ボーン行列を取得
	BoneTransform(_animTime);														
	
	//ブレンド行列or普通の行列をシェーダーに渡し、アニメーションを実行
	if (_isInBlend)	copy(_blendMats.begin(), _blendMats.end(), _mappedMats + 1);	
	else copy(_boneMats.begin(), _boneMats.end(), _mappedMats + 1);

	//前フレームの時間を更新
	_befFrameTime = _currFrameTime;
}

/// <summary>
/// 入力された方向へ移動する関数
/// </summary>
/// <param name="input">入力ベクトル</param>
void
FBXActor::Translate(const XMVECTOR& input)
{
	//座標に入力に応じたベクトルを加算
	_pos += input * _dx12.GetDeltaTime() * MOVE_SPEED;

	//入力ベクトルと正面ベクトルの角度差を取得
	auto diff = XMVector3AngleBetweenVectors(input, _frontVec).m128_f32[0];			
	diff = XMVector3Cross(input, _frontVec).m128_f32[1] > 0 ? diff * -1 : diff;
	//角度の差がある場合目的角度を更新
	if (FLT_EPSILON <= fabs(diff))													
	{
		_destRad = _rotY + diff;
	}

	//目的角度に向け線形補間
	_rotY = lerp(_rotY, _destRad, 0.2f);											

	//Z軸から正面ベクトルの角度差を取得
	//0～360の範囲にする
	_zToFrontAngle = XMVector3AngleBetweenVectors(Z_VECTOR, _frontVec).m128_f32[0];
	_zToFrontAngle = XMVector3Cross(Z_VECTOR, _frontVec).m128_f32[1] > 0 ? (2 * XM_PI) - _zToFrontAngle : _zToFrontAngle;
	_zToFrontAngle = _zToFrontAngle / XM_PI * 180.0f;

	//正面ベクトルを取得・正規化
	_frontVec = XMVectorSet(														
		_mappedMats[0].r[2].m128_f32[0],
		_mappedMats[0].r[2].m128_f32[1],
		_mappedMats[0].r[2].m128_f32[2],
		0.0f
	);
	_frontVec = XMVector3Normalize(_frontVec);
}

/// <summary>
/// 操作可能かどうかを設定する関数
/// </summary>
void
FBXActor::StartControll()
{
	//アニメーション再生時間も初期化
	_animTime = 0.0f;				
	//真理値を設定
	_canControll = true;			
	//アニメーションを待機モーションに設定
	SetAnimationNode(WAIT00);		
}

/// <summary>
/// 座標を初期化する関数
/// </summary>
void
FBXActor::EndControll()
{
	//単位行列を代入し、座標や正面ベクトルを初期化
	_mappedMats[0] = XMMatrixIdentity();				
	_pos = XMVectorZero();
	_frontVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	//角度、目的角度も初期化
	_rotY = 0.0f;
	_destRad = 0.0f;

	//真理値も初期化
	_canControll = false;					
	//ポーズも初期化
	InitPose();											
}

/// <summary>
/// アニメーション間のブレンドを実行する関数
/// </summary>
/// <param name="anim">アニメーション</param>
void
FBXActor::BlendAnimation(AnimEnum anim)
{
	//ブレンド用に前アニメーションの行列を保存
	_beforeMats = _boneMats;

	//アニメーションを変更
	_currentActorAnim = animations[anim];

	//再生時間を2つ目のフレームに設定
	_animTime = GetSecondFrame();

	//現アニメーションの行列を取得	
	BoneTransform(_animTime);

	//前アニメーション・現アニメーションの行列の線形補間処理を並列処理
	auto f = [&]()
	{
		_isInBlend = true;

		float t = 0.0f;
		for (int i = 0; i <= 60 * BLEND_SPEED; i++)
		{
			_blendWeight = lerp(1.0f, 0.0f, t);

			for (int i = 0; i < _blendMats.size(); i++)
			{
				_blendMats[i] =
					(_beforeMats[i] * _blendWeight) + (_boneMats[i] * (1 - _blendWeight));
			}
			t += _dx12.GetDeltaTime() / BLEND_SPEED;

			Sleep(1);
		}

		_isInBlend = false;
	};
	ParallelProcess(f);
}

/// <summary>
/// 操作可能かどうかを返す関数
/// </summary>
/// <returns>真理値</returns>
bool
FBXActor::GetCanControll()const
{
	return _canControll;
}

/// <summary>
/// アクターが持っているアニメーションの名前のベクトルを返す関数
/// </summary>
/// <returns>アニメーション名のベクトル</returns>
vector<string>
FBXActor::GetAnimstr()const
{
	return _animStr;
}

/// <summary>
/// 現在実行しているアニメーション名を返す関数
/// </summary>
/// <returns>アニメーション名</returns>
string
FBXActor::GetCurentAnimStr()const
{
	return _currentActorAnim;
}

/// <summary>
/// アニメーションの総時間を返す関数
/// </summary>
/// <param name="animation">アニメーション名</param>
/// <returns>総時間</returns>
float
FBXActor::GetAnimDuration(string animation)
{
	return static_cast<float>(_anims[CHARA_REF + animation]->mDuration);
}

/// <summary>
/// アニメーションの1秒当たりの処理回数を返す関数
/// </summary>
/// <returns>処理回数</returns>
float
FBXActor::GetAnimTickPerSpeed(string animation)
{
	//アニメーションの1秒当たりの処理回数が設定されていなかったら30を返す
	return static_cast<float>(_anims[CHARA_REF + animation]->mTicksPerSecond != 0
		? _anims[CHARA_REF + animation]->mTicksPerSecond : 30.0f);
}

/// <summary>
/// アニメーションの2個目のフレームを返す関数
/// </summary>
/// <returns>フレームの秒数</returns>
float
FBXActor::GetSecondFrame()
{
	return _secFrameTime;
}

/// <summary>
/// 実行するアニメーションを指定する関数
/// </summary>
/// <param name="animStr">アニメーション名</param>
void
FBXActor::SetAnimStr(string animStr)
{
	_currentActorAnim = animStr;
}

/// <summary>
/// アニメーションの再生速度を設定する
/// </summary>
/// <param name="speed">再生速度</param>
void 
FBXActor::SetAnimationSpeed(float speed)
{
	_animSpeed = speed;
}

/// <summary>
/// ImGui側からアニメーションの再生時間を貰う関数
/// </summary>
/// <param name="time">再生時間</param>
void
FBXActor::SetAnimationTime(float time)
{
	_animTime = time + GetSecondFrame();
}

/// <summary>
/// アニメーションを変更可能か決める関数
/// </summary>
/// <param name="val">真理値</param>
void
FBXActor::SetCanChangeAnim(bool val)
{
	_canChangeAnim = val;
}

/// <summary>
/// 地面と触れているか決める関数
/// </summary>
/// <param name="val">真理値</param>
void
FBXActor::SetOnGround(bool val)
{
	_isOnGround = val;
}

/// <summary>
/// アニメーションをループするか決める関数
/// </summary>
/// <param name="val">真理値</param>
void
FBXActor::SetIsInLoop(bool val)
{
	_isInLoop = val;
}

/// <summary>
/// 地面に接しているかどうかを返す
/// </summary>
/// <returns>真理値</returns>
bool
FBXActor::GetOnGround()const
{
	return _isOnGround;
}

/// <summary>
/// 現在のアニメーションを取得する関数
/// </summary>
/// <param name="anim">比較したいアニメーション</param>
/// <returns>引数と現在のアニメーションが等しいか</returns>
bool
FBXActor::IsAnimationEqual(AnimEnum anim)const
{
	if (!_crntNode)
	{
		return false;
	}
	else
	{
		return _crntNode->GetAnimEnum() == anim;
	}
}