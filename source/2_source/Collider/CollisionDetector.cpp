#include "Vector3.h"

#include "Collider/BoxCollider.h"
#include "Collider/CapsuleCollider.h"
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
			float s = LenOnOtherVec(diff,tempBox->DirectionVectors()[i]) / tempBox->HalfLength()[i];

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
	Vector3 diffBetColAndVec = GetDiffBetweenColAndVec(col, currentPos, nextPos);

	//現在と1フレーム後の座標のベクトルがOBBと交わる（=1フレーム後に衝突する）なら真
	if (XMVector3Length(diffBetColAndVec).m128_f32[0] > 0.0f)
	{
		return true;
	}

	return false;
}

/// <summary>
/// 二つのOBBが衝突しているか確認する関数
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
bool 
CollisionDetector::CheckColAndVec(
	shared_ptr<ICollider> col,
	const Vector3& startPos,
	const Vector3& endPos)
{
	//線分
	Vector3 line = endPos - startPos;
	//OBBの中心と線分の終端の差分
	Vector3 diffBetCenterAndEnd = endPos - *col->Center();

	if (col == dynamic_pointer_cast<BoxCollider>(col))
	{
		auto tempBox = dynamic_pointer_cast<BoxCollider>(col);

		float diffBetCenterAndEndLenOnDir, halfLen, lineLenOnDir;

		//OBBの方向ベクトルに対し差分を投影し、半分長と線分の投影の和を比較して衝突判定
		for (int i = 0; i < 3; i++)
		{
			//中心差分の投影
			diffBetCenterAndEndLenOnDir = LenOnOtherVec(diffBetCenterAndEnd, tempBox->DirectionVectors()[i]);
			//線分の方向ベクトルへの投影
			lineLenOnDir = LenOnOtherVec(line, tempBox->DirectionVectors()[i]);
			//方向ベクトルの半分長
			halfLen = tempBox->HalfLength()[i];

			if (diffBetCenterAndEndLenOnDir > halfLen + lineLenOnDir)
			{
				return false;
			}
		}
	}
	else if (col == dynamic_pointer_cast<SphereCollider>(col))
	{
		return false;
	}

	return true;
}

/// <summary>
/// カプセルと箱の当たり判定を確認する関数
/// </summary>
/// <param name="box">箱の当たり判定</param>
/// <param name="capsule">箱の当たり判定</param>
/// <returns>衝突しているかどうか</returns>
bool
CollisionDetector::CheckCapsuleAndBox(shared_ptr<ICollider> box, shared_ptr<ICollider> capsule)
{
	auto boxColl = dynamic_pointer_cast<BoxCollider>(box);
	auto capsuleColl = dynamic_pointer_cast<CapsuleCollider>(capsule);

	auto vecBetCenter = *boxColl->Center() - *capsuleColl->Center();
	
	if (fabs(XMVector3Dot(XMVector3Normalize(vecBetCenter), Vector3(0, 1, 0)).m128_f32[0]) > .9f)
	{
		return false;
	}

	auto lenBetCenter = XMVector3Length(vecBetCenter).m128_f32[0];
	ImGuiManager::Instance().AddLabelAndFloat("LenBetCenter", lenBetCenter);

	////①カプセルの上端、下端ベクトルをOBBのローカル空間に変換
	//Vector3 upToLocal = Vector3(
	//	LenOnOtherVec(*capsuleColl->UpEdge(), boxColl->DirectionVectors()[0]),
	//	LenOnOtherVec(*capsuleColl->UpEdge(),boxColl->DirectionVectors()[1]),
	//	LenOnOtherVec(*capsuleColl->UpEdge(),boxColl->DirectionVectors()[2])
	//) - *boxColl->Center();
	//Vector3 downToLocal = Vector3(
	//	LenOnOtherVec(*capsuleColl->DownEdge(), boxColl->DirectionVectors()[0]),
	//	LenOnOtherVec(*capsuleColl->DownEdge(), boxColl->DirectionVectors()[1]),
	//	LenOnOtherVec(*capsuleColl->DownEdge(), boxColl->DirectionVectors()[2])
	//) - *boxColl->Center();

	//ImGuiManager::Instance().AddLabelAndVector3("Up", upToLocal);
	//ImGuiManager::Instance().AddLabelAndVector3("Center", *capsuleColl->Center());
	//ImGuiManager::Instance().AddLabelAndVector3("Down", downToLocal);

	////②2つの当たり判定の中心を結ぶベクトル
	//Vector3 vecBetCenter = downToLocal - upToLocal;

	////③
	//float min = 0.0f;
	//float max = 1.0f;

	//int count = 0;
	//float candidates[8];
	//candidates[count++] = 0.0f;
	//candidates[count++] = 1.0f;

	//for (int i = 0; i < 3; ++i)
	//{
	//	//中心線と始点の要素をそれぞれ取得
	//	float directionComponent = vecBetCenter[i];
	//	float originComponent = upToLocal[i];

	//	//中心線の要素がある程度大きければ
	//	if (fabs(directionComponent) > 1e-6f)
	//	{
	//		float t1 = (-boxColl->HalfLength()[i] - originComponent) / directionComponent;
	//		float t2 = (boxColl->HalfLength()[i] - originComponent) / directionComponent;

	//		if (t1 > 0.0f && t1 < 1.0f)
	//		{
	//			candidates[count++] = t1;
	//		}
	//		if (t2 > 0.0f && t2 < 1.0f)
	//		{
	//			candidates[count++] = t2;
	//		}
	//	}
	//}

	//auto SqDistPointAABB = [&](Vector3 p)
	//{
	//	float ret = 0.0f;
	//	if (p.X() < -boxColl->HalfLength()[0]) ret += (-boxColl->HalfLength()[0] - p.X()) * (-boxColl->HalfLength()[0] - p.X());
	//	if (p.X() > boxColl->HalfLength()[0]) ret += (p.X() - boxColl->HalfLength()[0]) * (p.X() - boxColl->HalfLength()[0]);
	//	if (p.Y() < -boxColl->HalfLength()[1]) ret += (-boxColl->HalfLength()[1] - p.Y()) * (-boxColl->HalfLength()[1] - p.Y());
	//	if (p.Y() > boxColl->HalfLength()[1]) ret += (p.Y() - boxColl->HalfLength()[1]) * (p.Y() - boxColl->HalfLength()[1]);
	//	if (p.Z() < -boxColl->HalfLength()[2]) ret += (-boxColl->HalfLength()[2] - p.Z()) * (-boxColl->HalfLength()[2] - p.Z());
	//	if (p.Z() > boxColl->HalfLength()[2]) ret += (p.Z() - boxColl->HalfLength()[2]) * (p.Z() - boxColl->HalfLength()[2]);
	//	return ret;
	//};

	//float minSqDist = 1e30f;
	//for (int i = 0; i < count; i++)
	//{
	//	Vector3 p = upToLocal + vecBetCenter * candidates[i];
	//	float sqDistPointAABB = SqDistPointAABB(p);

	//	minSqDist = min(minSqDist, sqDistPointAABB);
	//}

	//ImGuiManager::Instance().AddLabelAndFloat("MinSqDist", sqrt(minSqDist));

	//return sqrt(minSqDist) >= capsuleColl->Radius();
	return false;
}

/// <summary>
/// 当たり判定に線分が入っているか確認する関数
/// </summary>
/// <param name="col">当たり判定</param>
/// <param name="startPos">線分の始点</param>
/// <param name="endPos">線分の終点</param>
/// <returns>入っているかどうか</returns>
Vector3
CollisionDetector::GetDiffBetweenColAndVec(shared_ptr<ICollider> col, const Vector3& startPos, const Vector3& endPos)
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
			lenOnDiffBetCenter = LenOnOtherVec(diffBetCenter, tempBox->DirectionVectors()[i]);
			//方向ベクトルの半分長
			halfLen = tempBox->HalfLength()[i];
			//線分の方向ベクトルへの投影
			lineLenOnDir = LenOnOtherVec(line, tempBox->DirectionVectors()[i]);

			if (lenOnDiffBetCenter > halfLen + lineLenOnDir)
			{
				return XMVectorZero();
			}
			else
			{
				colPoint[i] = lenOnDiffBetCenter - lineLenOnDir;
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
/// ベクトルとColliderの衝突判定を行う
/// スラブ法の使用例
/// </summary>
/// <param name="col">Collider</param>
/// <param name="startPos">ベクトルの始点</param>
/// <param name="direction">ベクトルの向き</param>
/// <param name="length">ベクトルの長さ</param>
/// <returns>衝突判定</returns>
bool 
CollisionDetector::CheckColAndVector(shared_ptr<ICollider> col, const Vector3 startPos, const Vector3 direction, float length)
{
	//カプセル・球の当たり判定は除外
	if (dynamic_pointer_cast<CapsuleCollider>(col) || dynamic_pointer_cast<SphereCollider>(col))
	{
		return false;
	}

	auto boxCol = dynamic_pointer_cast<BoxCollider>(col);

	//当たり判定の中心とベクトルの始点を結ぶベクトル
	Vector3 vecBetCenterAndStart = *boxCol->Center() - startPos;

	float min = 0.0f;
	float max = FLT_MAX;

	for (int i = 0; i < 3; i++)
	{
		//中心ベクトル、ベクトルの向きの各軸への投影
		float e = XMVector3Dot(boxCol->DirectionVectors()[i],vecBetCenterAndStart).m128_f32[0];
		float f = XMVector3Dot(boxCol->DirectionVectors()[i], direction).m128_f32[0];

		//ベクトルが軸に対し並行でない場合
		if (abs(f) > 1e-6f)
		{
			//軸でのベクトルに対する中心ベクトルと半分長の和の比
			float val_min = (e - boxCol->HalfLength()[i]) / f;
			//差の比
			float val_max = (e - boxCol->HalfLength()[i]) / f;

			//和の比が差の比より大きかったら入れ替え
			if (val_min > val_max)
			{
				swap(val_min, val_max);
			}

			//最小値、最大値の幅を狭めていく
			if (val_min > min)
			{
				min = val_min;
			}
			if (val_max < max)
			{
				max = val_max;
			}

			//最小値が最大値を上回るか、最大値が0未満になったら衝突していない
			if (min > max || max < 0.0f)
			{
				return false;
			}
		}
		//軸に対し平行な場合
		else
		{
			float s = boxCol->HalfLength()[i];
			if (-e - s > 0.0f || -e + s < 0.0f)
			{
				return false;
			}
		}

		return true;
	}
}

/// <summary>
/// 矩形同士の当たり判定を確認する関数
/// 分離軸定理の使用例
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
			r += LenOnOtherVec(tempBox1->DirectionVectors()[i],
				tempBox2->DirectionVectors()[j] * tempBox2->HalfLength()[j]);
		}

		//中心間ベクトルの分離軸への投影
		float s = LenOnOtherVec(vecBetCenter, tempBox1->DirectionVectors()[i]);

		//比較して、中心間ベクトルの投影の方が大きければ衝突していない
		if (s > r + tempBox1->HalfLength()[i])
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
	if(!dynamic_pointer_cast<BoxCollider>(col1) || !dynamic_pointer_cast<SphereCollider>(col2))
	{
		if (!dynamic_pointer_cast<SphereCollider>(col1) && !dynamic_pointer_cast<SphereCollider>(col2))
		{
			return false;
		}
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
/// 別ベクトルに投影されたベクトルの長さを取得する関数
/// </summary>
/// <param name="sep">長さを求めたいベクトル</param>
/// <param name="dirVec">投影先のベクトル</param>
/// <returns>投影ベクトル上のベクトルの大きさ</returns>
float
CollisionDetector::LenOnOtherVec(const Vector3& sep, const Vector3& dirVec)
{
	//分離軸と各ベクトルの内積を取り、その絶対値が投影された大きさとなる
	float ret = fabs(XMVector3Dot(sep, dirVec).m128_f32[0]);

	return ret;
}