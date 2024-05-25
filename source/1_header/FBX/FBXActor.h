#pragma once
#include "FBX/AnimNodes/Animations.h"
#include "FBX/AnimNodes/AnimNode.h"
#include "FBX/AnimNodes/WAIT00Node.h"
#include "FBX/AnimNodes/RUN00_FNode.h"
#include "FBX/AnimNodes/JUMP00Node.h"
#include "FBX/FBXBase.h"

/// <summary>
/// ゲーム画面上に登場する3Dオブジェクトのクラス（ボーン入り）
/// </summary>
class Dx12Wrapper;
class AssimpLoader;
class ImGuiManager;
class AnimNode;
class WAIT00Node;
class RUN00_FNode;
class JUMP00Node;
class FBXActor : public FBXBase
{	
	template<typename T>using ComPtr = ComPtr<T>;

	friend ImGuiManager;
	friend AnimNode;
	friend WAIT00Node;
	friend RUN00_FNode;
	friend JUMP00Node;

private:

	//アニメーション実行用ノードの配列
	unique_ptr<AnimNode> _animNodes[Length];
	//現在実行するノード
	AnimNode* _crntNode;												

	//ボーン行列のベクトル
	vector<XMMATRIX> _boneMats;	
	//ブレンド時の遷移前モーションの行列
	vector<XMMATRIX> _beforeMats;
	//アクターの座標系からワールド座標系に戻す転置行列
	vector<XMMATRIX> _blendMats;			
	vector<XMMATRIX> _invMats;	

	//アニメーションとその名前の連想配列
	map<string, aiAnimation*> _anims;										

	//現在再生しているアニメーション名
	string _currentActorAnim;		
	//アニメーション名のベクトル
	vector<string> _animStr;												

	//正面ベクトル
	XMVECTOR _frontVec;			

	//操作可能かを判別する真理値
	bool _canControll;			
	//ブレンド中かを判別する真理値
	bool _isInBlend;														

	//地面の上にいるかを判別する真理値
	bool _isOnGround;														

	//アニメーションをループさせるか決める真理値
	bool _isInLoop;	
	//アニメーションが変更可能か決める真理値
	bool _canChangeAnim;													

	//最終的なアクターの回転角（Y軸）
	float _destRad;	
	//モーションブレンド時に使用するウェイト値
	float _blendWeight;	
	//アニメーションの再生時間
	float _animTime;			
	//アニメーションの第2フレーム（Tポーズでないポーズの最初のフレーム）の秒数
	float _secFrameTime;			
	//アクターの向き（Y軸）
	float _rotY;															

	//前フレームの時間
	LARGE_INTEGER _befFrameTime;									
	//現フレームの時間
	LARGE_INTEGER _currFrameTime;											

	//ノード階層を読み込む関数
	void ReadNodeHeirarchy(float animationTime,const aiNode* pNode, const XMMATRIX& parentTrans);

	//aiAnimationからノード名が一致したaiNodeAnimを取り出す関数
	const aiNodeAnim* FindNodeAnim(const aiAnimation* animation, const string nodeName);

	//スケーリングアニメーションを補完する関数
	XMMATRIX CalcInterpolatedScaling(float animationTime,const aiNodeAnim* nodeAnim);
	//回転アニメーションを補完する関数
	XMMATRIX CalcInterpolatedRotation(float animationTime, const aiNodeAnim* nodeAnim);
	//座標移動アニメーションを補完する関数
	XMMATRIX CalcInterpolatedPosition(float animationTime, const aiNodeAnim* nodeAnim);

	//スケーリングアニメーションのキーを取り出す関数
	int FindScaling(float animationTime, const aiNodeAnim* nodeAnim);
	//回転アニメーションのキーを取り出す関数
	int FindRotation(float animationTime,const aiNodeAnim* nodeAnim);		
	//座標移動アニメーションのキーを取り出す関数
	int FindPosition(float animationTime,const aiNodeAnim* nodeAnim);		

	//アニメーション関連の初期化を行う関数
	void InitAnimation();													

	//モデルのアニメーション用行列を求める関数
	void BoneTransform(float timeInTicks);									

	//座標変換用バッファー・ビュー作成関数
	HRESULT CreateTransformView()override;									
public:	
	//コンストラクタ
	FBXActor(Dx12Wrapper& dx12, const wchar_t* filePath,XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f));
	//デストラクタ
	~FBXActor();														

	//アニメーション名のベクトルを返す関数
	vector<string> GetAnimstr()const;
	//現在実行しているアニメーション名を返す関数
	string GetCurentAnimStr()const;										

	//アニメーションの総時間を返す関数
	float GetAnimDuration(string animation);	
	//アニメーションの1秒当たりの処理回数を返す関数
	float GetAnimTickPerSpeed(string animation);
	//アニメーションの2個目のフレームを返す関数
	float GetSecondFrame();												

	//実行するアニメーション名を指定する関数
	void SetAnimStr(string animStr);			
	//アニメーションの再生速度を設定する関数
	void SetAnimationTime(float time);					
	//アクターをTポーズにする関数
	void InitPose();													

	//毎フレームの座標変換処理
	void Update()override;												

	//入力された方向へ移動する関数
	void Translate(const XMVECTOR& vec);								

	//操作開始時に実行する関数
	void StartControll();				
	//初期化関数
	void EndControll();													
	//操作可能かどうかを返す関数
	bool GetCanControll()const;											

	//アニメーションが引数に等しいか返す関数
	bool IsAnimationEqual(AnimEnum anim);								
	//アニメーション間のブレンドを実行する関数
	void SetAnimationNode(AnimEnum anim);		
	//アニメーションをセットする関数
	void BlendAnimation(AnimEnum anim);									

	//アニメーション実行可能か設定する
	void SetCanChangeAnim(bool val);
	//地面に触れているか設定する
	void SetOnGround(bool val);
	//アニメーションループ可能か設定する
	void SetIsInLoop(bool val);
};