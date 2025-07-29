#pragma once
#include "FBX/AnimNodes/Animations.h"
#include "FBX/IFbx.h"

/// <summary>
/// ゲーム画面上に登場する3Dオブジェクトのクラス（ボーン入り）
/// </summary>
class aiAnimation;
class aiNode;
class aiNodeAnim;
class AnimNode;
class AssimpLoader;
class BoxCollider;
class Dx12Wrapper;
class FbxComposition;
class ICollider;
class ImGuiManager;
class Vector3;
class FbxActor : public IFbx
{	
	template<typename T>using ComPtr = ComPtr<T>;

private:
	//着地判定を検出するための当たり判定
	shared_ptr<ICollider> _colForGround;

	//アニメーション実行用ノードの配列
	unique_ptr<AnimNode> _animNodes[Length];
	//現在実行するノード
	AnimNode* _crntNode = nullptr;

	//FBXクラスの共通処理をまとめたインスタンス
	shared_ptr<FbxComposition> _fbxComp;

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
	Vector3 _currentFrontVec = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

	//入力ベクトル
	Vector3 _inputVec;

	//操作可能かを判別する真理値
	bool _canControll = false;			
	//ブレンド中かを判別する真理値
	bool _isInBlend = false;												

	//アニメーションをループさせるか決める真理値
	bool _isInLoop = true;	
	//アニメーションが変更可能か決める真理値
	bool _canChangeAnim = true;													

	//最終的なアクターの回転角（Y軸）
	float _destRad = 0.0f;
	//モーションブレンド時に使用するウェイト値
	float _blendWeight = 0.0f;	
	//アニメーションの再生速度
	float _animSpeed = 1.0f;
	//アニメーションの再生時間
	float _animTime = 0.0f;			
	//アニメーションの第2フレーム（Tポーズでないポーズの最初のフレーム）の秒数
	float _secFrameTime;			
	//アクターの向き（Y軸）
	float _rotY = 0.0f;

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
	//アクターが地面の上にいるか判別するためのコールバック
	function<bool(const Vector3& vec)> _isOnGround;

	//コンストラクタ
	FbxActor() = default;
	//デストラクタ
	~FbxActor() = default;

	HRESULT Init(const wchar_t* filePath, const string name,
		const Vector3& size, const Vector3& pos, ColliderType colType)override;

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
	void SetAnimationSpeed(float speed);
	//アニメーションの再生時間を設定する関数
	void SetAnimationTime(float time);					
	//アクターをTポーズにする関数
	void InitPose();	

	//描画処理
	void Draw()override;

	//毎フレームの座標変換処理
	void Update()override;

	//キーが入力された時に呼び出す関数
	void OnKeyPressed(const Vector3& input);

	void AdjustPos(const Vector3& diff);

	//操作開始時に実行する関数
	void StartControll();				
	//初期化関数
	void EndControll();													
	//操作可能かどうかを返す関数
	bool GetCanControll()const;											

	//アニメーションが引数に等しいか返す関数
	bool IsAnimationEqual(AnimEnum anim)const;								
	//アニメーション間のブレンドを実行する関数
	void SetAnimationNode(AnimEnum anim);		
	//アニメーションをセットする関数
	void BlendAnimation(AnimEnum anim);									

	//アニメーション実行可能か設定する
	void SetCanChangeAnim(bool val);
	//アニメーションループ可能か設定する
	void SetIsInLoop(bool val);

	//現在の座標を取得
	Vector3 CurrentPosition()override;

	//本体の当たり判定を返す
	shared_ptr<ICollider>  Collider();

	//接地判定を行うOBBを返す
	shared_ptr<ICollider> GetColForGround()const;

	//地面に接しているかどうかを返す
	bool GetOnGround()const;
};