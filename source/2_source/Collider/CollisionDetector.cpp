#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CollisionDetector.h"
#include "Collider/ICollider.h"
#include "Collider/SphereCollider.h"
#include "FBX/FbxActor.h"
#include "FBX/IFbx.h"
#include "FBX/FbxObject.h"
#include "Manager/ImGuiManager.h"
#include "Wrapper/Dx12Wrapper.h"

/// <summary>
/// シングルトンを返す
/// </summary>
/// <returns>シングルトン</returns>
CollisionDetector&
CollisionDetector::Instance()
{
	static CollisionDetector instance;
	return instance;
}

/// <summary>
/// OBBと座標の間の距離を返す関数
/// </summary>
/// <param name="col">OBB</param>
/// <param name="pos">座標</param>
/// <returns>距離</returns>
float 
CollisionDetector::GetLengthBetweenColAndPos(shared_ptr<ICollider> col, const Vector3& pos)
{
	//座標とOBBの中心の差分
	Vector3 diff = pos - *col->Center();

	//差分の距離
	float lenOnDiff = XMVector3Length(diff).m128_f32[0];

	if(col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		//OBBの方向ベクトルのベクトルへの投影の和を取得
		float sumOfLenOnDir = 0.0f;
		for (int i = 0; i < 3; i++)
		{
			float s = fabs(XMVector3Dot(diff, tempBox->DirectionVectors()[i]).m128_f32[0]) / tempBox->HalfLength()[i];

			if (s > 1)
			{
				sumOfLenOnDir += (1 - s) * tempBox->HalfLength()[i];
			}
		}

		//OBB内の距離と差分の差
		float lenBetColAndPos = lenOnDiff - sumOfLenOnDir;

		return lenBetColAndPos;
	}
	else
	{
		auto tempSphere = dynamic_pointer_cast<SphereCollider>(col);

		return lenOnDiff - tempSphere->Radius();
	}
}

/// <summary>
/// 連続的な衝突判定処理
/// </summary>
/// <param name="actor">座標をズラす対象のアクター</param>
/// <param name="col">衝突対象のOBB</param>
/// <param name="dir">移動方向</param>
/// <param name="pos">現在の座標</param>
/// <param name="speed">移動速度</param>
/// <returns>衝突するかどうか</returns>
bool 
CollisionDetector::CheckContinuousCollisionDetection(
	IFbx* actor,
	shared_ptr<ICollider> col, 
	const Vector3& dir, 
	const Vector3& currentPos, 
	const float speed)
{
	//1フレーム後の座標を取得
	Vector3 nextPos = currentPos + (dir * speed * Dx12Wrapper::Instance().GetDeltaTime());
	//OBBと座標の差
	Vector3 diffBetColAndVec = CheckColAndVec(col, currentPos, nextPos);

	//現在と1フレーム後の座標のベクトルがOBBと交わる（=1フレーム後に衝突する）なら真
	if (XMVector3Length(diffBetColAndVec).m128_f32[0] > 0.0f)
	{
		return true;
	}

	return false;
}

/// <summary>
/// 分離軸定理二つのOBBが衝突しているか確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているかどうか</returns>
bool
CollisionDetector::CheckColAndCol(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2)
{
	Vector3 vecBetCenter = *col2->Center() - *col1->Center();

	return CheckBoxAndBox(col1, col2, vecBetCenter) || 
		CheckBoxAndSphere(col2, col1, vecBetCenter) || 
		CheckSphereAndSphere(col1, col2, vecBetCenter);
}

/// <summary>
/// 当たり判定に線分が入っているか確認する関数
/// </summary>
/// <param name="col">当たり判定</param>
/// <param name="startPos">線分の始点</param>
/// <param name="endPos">線分の終点</param>
/// <returns>入っているかどうか</returns>
Vector3
CollisionDetector::CheckColAndVec(shared_ptr<ICollider> col, const Vector3& startPos, const Vector3& endPos)
{
	//線分
	Vector3 line = endPos - startPos;
	//OBBと線分の中心の差分
	Vector3 diffBetCenter = endPos - *col->Center();
	//OBBと線分が交わる座標
	Vector3 colPoint = XMVectorZero();

	if (col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		float lenOnDiffBetCenter, halfLen, lineLenOnDir;
		//OBBの方向ベクトルに対し中心差分を投影し、半分長と線分の投影の和を比較して衝突判定
		for (int i = 0; i < 3; i++)
		{
			//中心差分の投影
			lenOnDiffBetCenter = XMVector3Dot(diffBetCenter, tempBox->DirectionVectors()[i]).m128_f32[0];
			//方向ベクトルの半分長
			halfLen = tempBox->HalfLength()[i];
			//線分の方向ベクトルへの投影
			lineLenOnDir = abs(XMVector3Dot(line, tempBox->DirectionVectors()[i]).m128_f32[0]);

			if (abs(lenOnDiffBetCenter) > halfLen + lineLenOnDir)
			{
				return XMVectorZero();
			}
			else
			{
				colPoint[i] = lenOnDiffBetCenter - lineLenOnDir;
				//if (i == 1)
				//{
				//	colPoint[i] = halfLen;
				//}
			}

		}
	}
	else if(col == dynamic_pointer_cast<SphereCollider>(col))
	{
		return XMVectorZero();
	}

	return colPoint + *col->Center();
}

/// <summary>
/// 矩形同士の当たり判定を確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているか</returns>
bool
CollisionDetector::CheckBoxAndBox(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<BoxCollider>(col1) || !dynamic_pointer_cast<BoxCollider>(col2))
	{
		return false;
	}

	auto tempBox1 = dynamic_pointer_cast<BoxCollider>(col1);
	auto tempBox2 = dynamic_pointer_cast<BoxCollider>(col2);

	for (int i = 0; i < 3; i++)
	{
		//各方向ベクトルの分離軸への投影の和
		float r = 0;
		for (int j = 0; j < 3; j++)
		{
			r += LenOnSeparateAxis(tempBox1->DirectionVectors()[i],
				tempBox2->DirectionVectors()[j] * tempBox2->HalfLength()[j]);
		}

		//中心間ベクトルの分離軸への投影
		float s = XMVector3Dot(vecBetCenter, tempBox1->DirectionVectors()[i]).m128_f32[0];

		//比較して、中心間ベクトルの投影の方が大きければ衝突していない
		if (fabs(s) > r + tempBox1->HalfLength()[i])
		{
			return false;
		}
	}

	return true;
}

/// <summary>
/// 球形同士の当たり判定を確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているか</returns>
bool
CollisionDetector::CheckSphereAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<SphereCollider>(col1) || !dynamic_pointer_cast<SphereCollider>(col2))
	{
		return false;
	}

	//球の半径の和を取得し、中心ベクトルの距離と比較
	auto tempSphe1 = dynamic_pointer_cast<SphereCollider>(col1);
	auto tempSphe2 = dynamic_pointer_cast<SphereCollider>(col2);
	auto sumOfRadius = tempSphe1->Radius() + tempSphe2->Radius();

	if (pow(sumOfRadius,2) >= XMVector3LengthSq(vecBetCenter).m128_f32[0])
	{
		return true;
	}
	else
	{
		return false;
	}
}
/// <summary>
/// 矩形と球形の当たり判定を確認する関数
/// </summary>
/// <param name="col1">当たり判定その1</param>
/// <param name="col2">当たり判定その2</param>
/// <returns>衝突しているか</returns>
bool
CollisionDetector::CheckBoxAndSphere(shared_ptr<ICollider> col1, shared_ptr<ICollider> col2, Vector3 vecBetCenter)
{
	if (!dynamic_pointer_cast<BoxCollider>(col1) || !dynamic_pointer_cast<SphereCollider>(col2))
	{
		return CheckBoxAndSphere(col2, col1, vecBetCenter);
	}

	auto tempBox = dynamic_pointer_cast<BoxCollider>(col1);
	auto tempSphere = dynamic_pointer_cast<SphereCollider>(col2);

	auto dis = XMVector3Length(vecBetCenter).m128_f32[0];
	auto radius = tempSphere->Radius();

	float disInBox = GetLengthBetweenColAndPos(tempBox, *tempSphere->Center());

	if (fabs(disInBox) - fabs(dis) <= fabs(tempSphere->Radius()))
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// <summary>
/// 分離軸に対し投影されたベクトルの大きさを求める関数
/// </summary>
/// <param name="sep">分離軸</param>
/// <param name="dirVec">方向ベクトル</param>
/// <returns>分離軸上の方向ベクトルの大きさ</returns>
float
CollisionDetector::LenOnSeparateAxis(const Vector3& sep, const Vector3& dirVec)
{
	//分離軸と各ベクトルの内積を取り、その絶対値が投影された大きさとなる
	float ret = fabs(XMVector3Dot(sep, dirVec).m128_f32[0]);

	return ret;
}