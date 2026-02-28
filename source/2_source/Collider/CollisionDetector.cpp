#define NOMINMAX
#include "Vector3.h"

#include <algorithm>

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

	//OBBが属するローカル空間への変換関数
	auto WorldToLocal = [&](Vector3 worldPos) {
		Vector3 rel = worldPos - *boxColl->Center();
		return Vector3{
			rel.X() * boxColl->DirectionVectors()[0].X() + rel.Y() * boxColl->DirectionVectors()[0].Y() + rel.Z() * boxColl->DirectionVectors()[0].Z(),
			rel.X() * boxColl->DirectionVectors()[1].X() + rel.Y() * boxColl->DirectionVectors()[1].Y() + rel.Z() * boxColl->DirectionVectors()[1].Z(),
			rel.X() * boxColl->DirectionVectors()[2].X() + rel.Y() * boxColl->DirectionVectors()[2].Y() + rel.Z() * boxColl->DirectionVectors()[2].Z()
		};
	};

	//点とAABBの最短距離の二乗を求めるヘルパー
	auto SqDistPointAABB = [&](Vector3 p, Vector3 extents) {
		float sqDist = 0.0f;
		// X軸
		if (p.X() < -extents.X()) sqDist += (p.X() + extents.X()) * (p.X() + extents.X());
		else if (p.X() > extents.X()) sqDist += (p.X() - extents.X()) * (p.X() - extents.X());
		// Y軸
		if (p.Y() < -extents.Y()) sqDist += (p.Y() + extents.Y()) * (p.Y() + extents.Y());
		else if (p.Y() > extents.Y()) sqDist += (p.Y() - extents.Y()) * (p.Y() - extents.Y());
		// Z軸
		if (p.Z() < -extents.Z()) sqDist += (p.Z() + extents.Z()) * (p.Z() + extents.Z());
		else if (p.Z() > extents.Z()) sqDist += (p.Z() - extents.Z()) * (p.Z() - extents.Z());

		return sqDist;
	};

	//線分とAABBの距離の二乗を求める
	auto SegmentAABBSqDistance = [&](Vector3 p0, Vector3 p1, Vector3 extents)
	{
		Vector3 d = p1 - p0;
		float tMin = 0.0f;
		float tMax = 1.0f;

		// 各軸(X, Y, Z)について、線分がAABBの「外側」にある範囲を絞り込む
		// この処理は、線分をAABBの各面で作られるスラブでクリッピングするのと同義です。
		auto clip = [&](float p, float delta, float extent) {
			if (std::abs(delta) < 1e-7f) {
				// 線分が軸に平行な場合、その軸で外れていれば交差しない
				return p < -extent || p > extent;
			}
			float t0 = (-extent - p) / delta;
			float t1 = (extent - p) / delta;
			if (t0 > t1) std::swap(t0, t1);

			tMin = max(tMin, t0);
			tMax = min(tMax, t1);
			return tMin > tMax; // 矛盾が生じたら（この軸では外れている）
		};

		// 3軸すべてでクリッピングを試みる
		bool separated = clip(p0.X(), d.X(), extents.X()) ||
			clip(p0.Y(), d.Y(), extents.Y()) ||
			clip(p0.Z(), d.Z(), extents.Z());

		if (!separated) {
			// 線分の一部または全部がAABBの内部にある場合、最短距離は0
			// (厳密にはtMin〜tMaxの範囲で交差している)
			return 0.0f;
		}

		// 線分が外側にある場合、p0(t=0), p1(t=1), または
		// クリッピング境界(tMin, tMax)のいずれかがAABBに最も近い点になる
		float sqDistP0 = SqDistPointAABB(p0, extents);
		float sqDistP1 = SqDistPointAABB(p1, extents);
		float sqDistTMin = SqDistPointAABB(p0 + d * std::clamp(tMin, 0.0f, 1.0f), extents);
		float sqDistTMax = SqDistPointAABB(p0 + d * std::clamp(tMax, 0.0f, 1.0f), extents);

		return std::min({ sqDistP0, sqDistP1, sqDistTMin, sqDistTMax });
	};

	//1. 端点をローカル座標に変換（必ず両端を使う）
	Vector3 l0 = WorldToLocal(*capsuleColl->UpEdge());
	Vector3 l1 = WorldToLocal(*capsuleColl->DownEdge());
	Vector3 extents = boxColl->HalfLength();

	//2. 線分(l0 to l1)とAABB(extents)の最短距離の二乗を求める
	float sqDist = SegmentAABBSqDistance(l0, l1, extents);

	//3. 半径を考慮した判定
	float r = capsuleColl->Radius();
	return sqDist <= (r * r);
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
	else if (col == dynamic_pointer_cast<SphereCollider>(col))
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
CollisionDetector::CheckColAndVector(shared_ptr<ICollider> col, Vector3 startPos, Vector3 direction, float length)
{
	//カプセルの当たり判定はひとまず除外
	if (dynamic_pointer_cast<CapsuleCollider>(col))
	{
		return false;
	}

	//当たり判定の中心とベクトルの始点を結ぶベクトル
	Vector3 vecBetCenterAndStart = *col->Center() - startPos;

	//ベクトルをf=始点+t*方向として、OBBに入った時のtの値
	float min = 0.0f;
	//出た時の値
	float max = FLT_MAX;

	// 型判定
	auto boxCol = dynamic_pointer_cast<BoxCollider>(col);
	auto sphere = dynamic_pointer_cast<SphereCollider>(col);

	for (int i = 0; i < 3; i++)
	{
		float e, f, half;

		if (boxCol)
		{
			// OBBの場合：各面の法線ベクトルへ投影
			e = XMVector3Dot(boxCol->DirectionVectors()[i], vecBetCenterAndStart).m128_f32[0];
			f = XMVector3Dot(boxCol->DirectionVectors()[i], direction * length).m128_f32[0];
			half = boxCol->HalfLength()[i];
		}
		else if (sphere)
		{
			// Sphereの場合：ワールド座標軸(XYZ)へ投影（AABB近似）
			// ※directionは正規化されている前提、startPosはレイの始点
			e = (*sphere->Center())[i] - startPos[i];
			f = direction[i] * length;
			half = sphere->Radius();
		}
		else { return false; }

		// 平行判定
		if (abs(f) > 1e-6f)
		{
			float val_min = (e - half) / f;
			float val_max = (e + half) / f;

			if (val_min > val_max) swap(val_min, val_max);

			if (val_min > min) min = val_min;
			if (val_max < max) max = val_max;

			// 交差していない、またはレイの背後にある
			if (min > max || max < 0.0f) return false;
		}
		else
		{
			// 軸に平行な場合、始点が範囲外なら当たっていない
			if (-e - half > 0.0f || -e + half < 0.0f) return false;
		}
	}

	return max > 0;
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
		//if (!dynamic_pointer_cast<SphereCollider>(col1) && !dynamic_pointer_cast<SphereCollider>(col2))
		//{
			return false;
		//}
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