#pragma once
#include "Vector3.h"
#include "matrix44.h"
#include "Frustum.h"

namespace CU
{
	class Camera
	{
	public:
		Camera();
		Camera(const float aFov, const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar);
		// Initializes the camera with screen height, screen width and it's world position 
		// In the case of TGA2D both aProjectionWidth and aProjectionHeight should probably be 1. 
		// maybe it's the same in BSR? we'll see about that
		
		~Camera();

		//Init yo
		//Perspective
		void Init(const float aFov, const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar);

		//orthographic
		void Init(const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar);



		void ReInit(const float aFov, const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar);
		void ReInit(const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar);

		

		// Returns the inverse of the camera orientation 
		Matrix44f GetInverse() const;

		// Returns the projection matrix 
		const Matrix44f& GetProjection() const;
		const Matrix44f& GetProjectionInverse() const;
		inline const float GetProjectionWidth() const;
		inline const float GetProjectionHeight() const;

		const Vector3f GetPosition() const;
		void SetPosition(const CU::Vector3f& aPosition);

		void LookAt(const CU::Vector3f& aPosition);

		void Translate(Vector3f aVector);
		void Roll(const float aAngle);
		void Pitch(const float aAngle);
		void Jaw(const float aAngle);

		void TranslateForward(const float aAmount);
		void TranslateSideways(const float aAmount);

		void SetTransformation(const Matrix44f & aTransformation);
		const Matrix44f& GetTransformation() const;
		inline float GetFar();
		inline float GetNear();
		inline const CU::CFrustum& GetFrustum();
		
		bool IsInside(const CU::AABB& aAABB);

	private:
		// Describes the orientation of the camera in regard to world space (or 
		// rather its parent space, but it's probably world) 
		Matrix44f myTransformation;

		// Describes the transformation from camera space to screen space 
		Matrix44f myProjection;
		Matrix44f myProjectionInverse;

		//supercool frustum that will be glorious and amazing
		CFrustum myFrustum;

		float myWidth;
		float myHeight;
		float myNear;
		float myFar;
	};

	inline const CU::CFrustum & CU::Camera::GetFrustum()
	{
		return myFrustum;
	}

	float Camera::GetFar()
	{
		return myFar;
	}

	float Camera::GetNear()
	{
		return myNear;
	}

	inline const float CU::Camera::GetProjectionWidth() const
	{
		return myWidth;
	}

	inline const float Camera::GetProjectionHeight() const
	{
		return myHeight;
	}
} 
