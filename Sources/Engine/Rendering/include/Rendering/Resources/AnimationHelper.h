#pragma once
#include "Maths/FQuaternion.h"
#include "Maths/FVector3.h"
#include "vector"

#define VEC3_EPSILON 0.000001
namespace Rendering::Resources
{
	struct Transform
	{
		Maths::FVector3 position;
		Maths::FQuaternion rotation;
		Maths::FVector3 scale;

		Transform(const Maths::FVector3& p, const Maths::FQuaternion& r, const Maths::FVector3& s) :
			position(p), rotation(r), scale(s) {}

		Transform() :
			position(Maths::FVector3(0, 0, 0)),
			rotation(Maths::FQuaternion(0, 0, 0, 1)),
			scale(Maths::FVector3(1, 1, 1))
		{}

		static Transform combine(const Transform& a, const Transform& b) {
			Transform out;
			out.scale = a.scale * b.scale;
			out.rotation = b.rotation * a.rotation;
			out.position = a.rotation * (a.scale * b.position);
			out.position = a.position + out.position;
			return out;
		}

		Transform inverse(const Transform& t) {
			Transform inv;
			inv.rotation = Maths::FQuaternion::inverse(t.rotation);
			inv.scale.x = fabs(t.scale.x) < VEC3_EPSILON ?
				0.0f : 1.0f / t.scale.x;
			inv.scale.y = fabs(t.scale.y) < VEC3_EPSILON ?
				0.0f : 1.0f / t.scale.y;
			inv.scale.z = fabs(t.scale.z) < VEC3_EPSILON ?
				0.0f : 1.0f / t.scale.z;
			Maths::FVector3 invTrans = t.position * -1.0f;
			inv.position = inv.rotation * (inv.scale * invTrans);
			return inv;
		}

		Transform mix(const Transform& a, const Transform& b, float t) {
			Maths::FQuaternion bRot = b.rotation;
			if (Maths::FQuaternion::dotProduct(a.rotation, bRot) < 0.0f)
				bRot = -bRot;

			return Transform(
				Maths::FVector3::lerp(a.position, b.position, t),
				Maths::FQuaternion::nlerp(a.rotation, bRot, t),
				Maths::FVector3::lerp(a.scale, b.scale, t));
		}

		static Maths::FMatrix4 transformToMat4(const Transform& t) {
			// First, extract the rotation basis of the transform
			Maths::FVector3 x = t.rotation * Maths::FVector3(1, 0, 0);
			Maths::FVector3 y = t.rotation * Maths::FVector3(0, 1, 0);
			Maths::FVector3 z = t.rotation * Maths::FVector3(0, 0, 1);
			// Next, scale the basis vectors
			x = x * t.scale.x;
			y = y * t.scale.y;
			z = z * t.scale.z;
			// Extract the position of the transform
			Maths::FVector3 p = t.position;
			// Create matrix
			return Maths::FMatrix4(
				x.x, x.y, x.z, 0, // X basis (& Scale)
				y.x, y.y, y.z, 0, // Y basis (& scale)
				z.x, z.y, z.z, 0, // Z basis (& scale)
				p.x, p.y, p.z, 1  // Position
			);
		}

		Transform mat4ToTransform(const Maths::FMatrix4& m) {
			Transform out;
			out.position = Maths::FVector3(m.data[3][0], m.data[3][1], m.data[3][2]);
			out.rotation = Maths::FQuaternion(m);
			Maths::FMatrix4 rotScaleMat(
				m.data[0][0], m.data[0][1], m.data[0][2], 0,
				m.data[1][0], m.data[1][1], m.data[1][2], 0,
				m.data[2][0], m.data[2][1], m.data[2][2], 0,
				0, 0, 0, 1
			);
			Maths::FMatrix4 invRotMat = Maths::FQuaternion::toMatrix4(Maths::FQuaternion::inverse(out.rotation));
			Maths::FMatrix4 scaleSkewMat = rotScaleMat * invRotMat;
			out.scale = Maths::FVector3(
				scaleSkewMat.data[0][0],
				scaleSkewMat.data[1][1],
				scaleSkewMat.data[2][2]
			);
			return out;
		}

		Maths::FVector3 transformPoint(const Transform& a, const Maths::FVector3& b) {
			Maths::FVector3 out;
			out = a.rotation * (a.scale * b);
			out = a.position + out;
			return out;
		}

		Maths::FVector3 transformVector(const Transform& a, const Maths::FVector3& b) {
			Maths::FVector3 out;
			out = a.rotation * (a.scale * b);
			return out;
		}
	};

	template<typename T>
	class Bezier {
	public:
		T P1; // Point 1
		T C1; // Control 1
		T P2; // Point 2
		T C2; // Control 2

		template<typename T>
		inline T Interpolate(Bezier<T>& curve, float t) {
			return curve.P1 * ((1 - t) * (1 - t) * (1 - t)) +
				curve.C1 * (3.0f * ((1 - t) * (1 - t)) * t) +
				curve.C2 * (3.0f * (1 - t) * (t * t)) +
				curve.P2 * (t * t * t);
		}

		template<typename T>
		T Hermite(float t, T& p1, T& s1, T& p2, T& s2) {
			return
				p1 * ((1.0f + 2.0f * t) * ((1.0f - t) * (1.0f - t))) +
				s1 * (t * ((1.0f - t) * (1.0f - t))) +
				p2 * ((t * t) * (3.0f - 2.0f * t)) +
				s2 * ((t * t) * (t - 1.0f));
		}
	};

	enum class Interpolation {
		Constant,
		Linear,
		Cubic
	};

	template<unsigned int N>
	class Frame {
	public:
		float mValue[N];
		float mIn[N];
		float mOut[N];
		float mTime;
	};

	typedef Frame<1> ScalarFrame;
	typedef Frame<3> VectorFrame;
	typedef Frame<4> QuaternionFrame;


	template<typename T, int N>
	class Track {
	protected:
		std::vector<Frame<N>> mFrames;
		Interpolation mInterpolation;

	public:
		Track()
		{
			mInterpolation = Interpolation::Linear;
		}
		void Resize(unsigned int size) {
			mFrames.resize(size);
		}
		unsigned int Size() {
			return mFrames.size();
		}
		Interpolation GetInterpolation() {
			return mInterpolation;
		}
		void SetInterpolation(Interpolation interpolation) {
			mInterpolation = interpolation;
		}
		float GetStartTime() {
			return mFrames[0].mTime;
		}
		float GetEndTime() {
			return mFrames[mFrames.size() - 1].mTime;
		}

		T Sample(float time, bool looping) {
			if (mInterpolation == Interpolation::Constant) {
				return SampleConstant(time, looping);
			}
			else if (mInterpolation == Interpolation::Linear) {
				return SampleLinear(time, looping);
			}
			return SampleCubic(time, looping);
		}
		Frame<N>& operator[](unsigned int index) {
			return mFrames[index];
		}

		T SampleConstant(float t, bool loop) {
			int frame = FrameIndex(t, loop);
			if (frame < 0 || frame >= (int)mFrames.size()) {
				return T();
			}
			return Cast(&mFrames[frame].mValue[0]);
		}
		T SampleLinear(float time, bool looping) {
			int thisFrame = FrameIndex(time, looping);
			if (thisFrame < 0 || thisFrame >= mFrames.size() - 1) {
				return T();
			}
			int nextFrame = thisFrame + 1;
			float trackTime = AdjustTimeToFitTrack(time, looping);
			float thisTime = mFrames[thisFrame].mTime;
			float frameDelta = mFrames[nextFrame].mTime - thisTime;
			if (frameDelta <= 0.0f) {
				return T();
			}
			float t = (trackTime - thisTime) / frameDelta;
			T start = Cast(&mFrames[thisFrame].mValue[0]);
			T end = Cast(&mFrames[nextFrame].mValue[0]);
			return TrackHelpers::Interpolate(start, end, t);
		}
		T SampleCubic(float time, bool looping) {
			int thisFrame = FrameIndex(time, looping);
			if (thisFrame < 0 || thisFrame >= mFrames.size() - 1) {
				return T();
			}
			int nextFrame = thisFrame + 1;
			float trackTime = AdjustTimeToFitTrack(time, looping);
			float thisTime = mFrames[thisFrame].mTime;
			float frameDelta = mFrames[nextFrame].mTime - thisTime;
			if (frameDelta <= 0.0f) {
				return T();
			}
			float t = (trackTime - thisTime) / frameDelta;
			size_t fltSize = sizeof(float);
			T point1 = Cast(&mFrames[thisFrame].mValue[0]);
			T slope1;// = mFrames[thisFrame].mOut * frameDelta;
			memcpy(&slope1, mFrames[thisFrame].mOut, N * fltSize);
			slope1 = slope1 * frameDelta;
			T point2 = Cast(&mFrames[nextFrame].mValue[0]);
			T slope2;// = mFrames[nextFrame].mIn[0] * frameDelta;
			memcpy(&slope2, mFrames[nextFrame].mIn, N * fltSize);
			slope2 = slope2 * frameDelta;
			return Hermite(t, point1, slope1, point2, slope2);
		}

		T Hermite(float t, const T& p1, const T& s1,
			const T& _p2, const T& s2) {
			float tt = t * t;
			float ttt = tt * t;
			T p2 = _p2;
			TrackHelpers::Neighborhood(p1, p2);
			float h1 = 2.0f * ttt - 3.0f * tt + 1.0f;
			float h2 = -2.0f * ttt + 3.0f * tt;
			float h3 = ttt - 2.0f * tt + t;
			float h4 = ttt - tt;
			T result = p1 * h1 + p2 * h2 + s1 * h3 + s2 * h4;
			return TrackHelpers::AdjustHermiteResult(result);
		}

		int FrameIndex(float time, bool looping) {
			unsigned int size = (unsigned int)mFrames.size();
			if (size <= 1) {
				return -1;
			}
			if (looping) {
				float startTime = mFrames[0].mTime;
				float endTime = mFrames[size - 1].mTime;
				float duration = endTime - startTime;
				time = fmodf(time - startTime, endTime - startTime);
				if (time < 0.0f) {
					time += endTime - startTime;
				}
				time = time + startTime;
			}
			else {
				if (time <= mFrames[0].mTime) {
					return 0;
				}
				if (time >= mFrames[size - 2].mTime) {
					return (int)size - 2;
				}
			}
			for (int i = (int)size - 1; i >= 0; --i) {
				if (time >= mFrames[i].mTime) {
					return i;
				}
			}

			// Invalid code, we should not reach here!
			return -1;
		}
		float AdjustTimeToFitTrack(float time, bool looping){
			unsigned int size = (unsigned int)mFrames.size();
			if (size <= 1) {
				return 0.0f;
			}
			float startTime = mFrames[0].mTime;
			float endTime = mFrames[size - 1].mTime;
			float duration = endTime - startTime;
			if (duration <= 0.0f) {
				return 0.0f;
			}
			if (looping) {
				time = fmodf(time - startTime,
					endTime - startTime);
				if (time < 0.0f) {
					time += endTime - startTime;
				}
				time = time + startTime;
			}
			else {
				if (time <= mFrames[0].mTime) {
					time = startTime;
				}
				if (time >= mFrames[size - 1].mTime) {
					time = endTime;
				}
			}
			return time;
		}

		T Cast(float* value); // Will be specialized

	};

	template<> float Track<float, 1>::Cast(float* value) {
		return value[0];
	}
	template<> Maths::FVector3 Track<Maths::FVector3, 3>::Cast(float* value) {
		return Maths::FVector3(value[0], value[1], value[2]);
	}
	template<> Maths::FQuaternion Track<Maths::FQuaternion, 4>::Cast(float* value) {
		Maths::FQuaternion r = Maths::FQuaternion(value[0], value[1], value[2], value[3]);
		return Maths::FQuaternion::normalize(r);
	}

	typedef Track<float, 1> ScalarTrack;
	typedef Track<Maths::FVector3, 3> VectorTrack;
	typedef Track<Maths::FQuaternion, 4> QuaternionTrack;

	template Track<float, 1>;
	template Track<Maths::FVector3, 3>;
	template Track<Maths::FQuaternion, 4>;
}

namespace TrackHelpers {
	inline float Interpolate(float a, float b, float t) {
		return a + (b - a) * t;
	}
	inline Maths::FVector3 Interpolate(const Maths::FVector3& a, const Maths::FVector3& b,
		float t) {
		return Maths::FVector3::lerp(a, b, t);
	}
	inline Maths::FQuaternion Interpolate(const Maths::FQuaternion& a, const Maths::FQuaternion& b,
		float t) {
		Maths::FQuaternion result = Maths::FQuaternion::mix(a, b, t);
		if (Maths::FQuaternion::dotProduct(a, b) < 0) { // Neighborhood
			result = Maths::FQuaternion::mix(a, -b, t);
		}
		return Maths::FQuaternion::normalize(result); //NLerp, not slerp
	}
	inline float AdjustHermiteResult(float f) {
		return f;
	}
	inline Maths::FVector3 AdjustHermiteResult(const Maths::FVector3& v) {
		return v;
	}
	inline Maths::FQuaternion AdjustHermiteResult(const Maths::FQuaternion& q) {
		return Maths::FQuaternion::normalize(q);
	}
	inline void Neighborhood(const float& a, float& b) {}
	inline void Neighborhood(const Maths::FVector3& a, Maths::FVector3& b) {}
	inline void Neighborhood(const Maths::FQuaternion& a, Maths::FQuaternion& b) {
		if (Maths::FQuaternion::dotProduct(a, b) < 0) {
			b = -b;
		}
	}
}

namespace Rendering::Resources
{
	class TransformTrack {
	protected:
		unsigned int mId;
		VectorTrack mPosition;
		QuaternionTrack mRotation;
		VectorTrack mScale;
	public:
		TransformTrack(){
			mId = 0;
		}
		unsigned int GetId() {
			return mId;
		}
		void SetId(unsigned int id) {
			mId = id;
		}
		VectorTrack& GetPositionTrack() {
			return mPosition;
		}
		QuaternionTrack& GetRotationTrack() {
			return mRotation;
		}
		VectorTrack& GetScaleTrack() {
			return mScale;
		}
		float GetStartTime() {
			float result = 0.0f;
			bool isSet = false;
			if (mPosition.Size() > 1) {
				result = mPosition.GetStartTime();
				isSet = true;
			}
			if (mRotation.Size() > 1) {
				float rotationStart = mRotation.GetStartTime();
				if (rotationStart < result || !isSet) {
					result = rotationStart;
					isSet = true;
				}
			}
			if (mScale.Size() > 1) {
				float scaleStart = mScale.GetStartTime();
				if (scaleStart < result || !isSet) {
					result = scaleStart;
					isSet = true;
				}
			}
			return result;
		}
		float GetEndTime() {
			float result = 0.0f;
			bool isSet = false;
			if (mPosition.Size() > 1) {
				result = mPosition.GetEndTime();
				isSet = true;
			}
			if (mRotation.Size() > 1) {
				float rotationEnd = mRotation.GetEndTime();
				if (rotationEnd > result || !isSet) {
					result = rotationEnd;
					isSet = true;
				}
			}
			if (mScale.Size() > 1) {
				float scaleEnd = mScale.GetEndTime();
				if (scaleEnd > result || !isSet) {
					result = scaleEnd;
					isSet = true;
				}
			}
			return result;
		}
		bool IsValid() {
			return mPosition.Size() > 1 ||
				mRotation.Size() > 1 ||
				mScale.Size() > 1;
		}
		Transform Sample(const Transform& ref, float time, bool loop) {
			Transform result = ref; // Assign default values
			if (mPosition.Size() > 1) { // Only if valid
				result.position = mPosition.Sample(time, loop);
			}
			if (mRotation.Size() > 1) { // Only if valid
				result.rotation = mRotation.Sample(time, loop);
			}
			if (mScale.Size() > 1) { // Only if valid
				result.scale = mScale.Sample(time, loop);
			}
			return result;
		}
	};

	class Pose {
	protected:
		std::vector<Transform> mJoints;
		std::vector<int> mParents;
	public:
		Pose::Pose() { }
		Pose::Pose(unsigned int numJoints) {
			Resize(numJoints);
		}
		Pose::Pose(const Pose& p) {
			*this = p;
		}
		Pose& operator=(const Pose& p) {
			if (&p == this) {
				return *this;
			}
			if (mParents.size() != p.mParents.size()) {
				mParents.resize(p.mParents.size());
			}
			if (mJoints.size() != p.mJoints.size()) {
				mJoints.resize(p.mJoints.size());
			}
			if (mParents.size() != 0) {
				memcpy(&mParents[0], &p.mParents[0],
					sizeof(int) * mParents.size());
			}
			if (mJoints.size() != 0) {
				memcpy(&mJoints[0], &p.mJoints[0],
					sizeof(Transform) * mJoints.size());
			}
			return *this;
		}
		void Resize(unsigned int size) {
			mParents.resize(size);
			mJoints.resize(size);
		}
		unsigned int Size() {
			return mJoints.size();
		}
		int GetParent(unsigned int index) {
			return mParents[index];
		}
		void SetParent(unsigned int index, int parent) {
			mParents[index] = parent;
		}
		Transform GetLocalTransform(unsigned int index) {
			return mJoints[index];
		}
		void SetLocalTransform(unsigned int index, const Transform& transform) {
			mJoints[index] = transform;
		}
		Transform GetGlobalTransform(unsigned int i) {
			Transform result = mJoints[i];
			for (int p = mParents[i]; p >= 0; p = mParents[p]) {
				result = Transform::combine(mJoints[p], result);
			}
			return result;
		}
		Transform operator[](unsigned int index) {
			return GetGlobalTransform(index);
		}
		void GetMatrixPalette(std::vector<Maths::FMatrix4>& out) {
			unsigned int size = Size();
			if (out.size() != size) {
				out.resize(size);
			}
			for (unsigned int i = 0; i < size; ++i) {
				Transform t = GetGlobalTransform(i);
				out[i] = Transform::transformToMat4(t);
			}
		}
		bool operator==(const Pose& other) {
			if (mJoints.size() != other.mJoints.size()) {
				return false;
			}
			if (mParents.size() != other.mParents.size()) {
				return false;
			}
			unsigned int size = (unsigned int)mJoints.size();
			for (unsigned int i = 0; i < size; ++i) {
				Transform thisLocal = mJoints[i];
				Transform otherLocal = other.mJoints[i];
				int thisParent = mParents[i];
				int otherParent = other.mParents[i];
				if (thisParent != otherParent) { return false; }
				if (thisLocal.position != otherLocal.position) {
					return false;
				}
				if (thisLocal.rotation != otherLocal.rotation){
				return false; }
				if (thisLocal.scale != otherLocal.scale){
				return false; }
			}
			return true;
		}
		bool operator!=(const Pose& other) {
			return !(*this == other);
		}
	};

	class Clip {
	protected:
		std::vector<TransformTrack> mTracks;
		std::string mName;
		float mStartTime;
		float mEndTime;
		bool mLooping;
	protected:
		float AdjustTimeToFitRange(float inTime) {
			if (mLooping) {
				float duration = mEndTime - mStartTime;
				if (duration <= 0) 
				{ 0.0f; }
				
				inTime = fmodf(inTime - mStartTime, mEndTime - mStartTime);
				
				if (inTime < 0.0f) {
					inTime += mEndTime - mStartTime;
				}
				
				inTime = inTime + mStartTime;
			}
			else {
				if (inTime < mStartTime) {
					inTime = mStartTime;
				}
				if (inTime > mEndTime) {
					inTime = mEndTime;
				}
			}
			return inTime;
		}
	public:
		Clip() {
			mName = "No name given";
			mStartTime = 0.0f;
			mEndTime = 0.0f;
			mLooping = true;
		}
		unsigned int GetIdAtIndex(unsigned int index) {
			return mTracks[index].GetId();
		}
		void SetIdAtIndex(unsigned int index, unsigned int id) {
			return mTracks[index].SetId(id);
		}
		unsigned int Size() {
			return (unsigned int)mTracks.size();
		}
		float Sample(Pose& outPose, float time) {
			if (GetDuration() == 0.0f) {
				return 0.0f;
			}
			time = AdjustTimeToFitRange(time);
			unsigned int size = mTracks.size();
			for (unsigned int i = 0; i < size; ++i) {
				unsigned int j = mTracks[i].GetId(); // Joint
				Transform local = outPose.GetLocalTransform(j);
				Transform animated = mTracks[i].Sample(
					local, time, mLooping);
				outPose.SetLocalTransform(j, animated);
			}
			return time;
		}
		TransformTrack& operator[](unsigned int joint) {
			for (int i = 0, s = mTracks.size(); i < s; ++i) {
				if (mTracks[i].GetId() == joint) {
					return mTracks[i];
				}
			}
			mTracks.push_back(TransformTrack());
			mTracks[mTracks.size() - 1].SetId(joint);
			return mTracks[mTracks.size() - 1];
		}
		void RecalculateDuration() {
			mStartTime = 0.0f;
			mEndTime = 0.0f;
			bool startSet = false;
			bool endSet = false;
			unsigned int tracksSize = mTracks.size();
			for (unsigned int i = 0; i < tracksSize; ++i) {
				if (mTracks[i].IsValid()) {
					float startTime = mTracks[i].GetStartTime();
					float endTime = mTracks[i].GetEndTime();
					if (startTime < mStartTime || !startSet) {
						mStartTime = startTime;
						startSet = true;
					}
					if (endTime > mEndTime || !endSet) {
						mEndTime = endTime;
						endSet = true;
					}
				}
			}
		}
		std::string& GetName() {
			return mName;
		}
		void SetName(const std::string& inNewName) {
			mName = inNewName;
		}
		float GetDuration() {
			return mEndTime - mStartTime;
		}
		float GetStartTime() {
			return mStartTime;
		}
		float GetEndTime() {
			return mEndTime;
		}
		bool GetLooping() {
			return mLooping;
		}
		void SetLooping(bool inLooping) {
			mLooping = inLooping;
		}
	};
}