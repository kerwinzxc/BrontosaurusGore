#include "stdafx.h"
#include "Camera.h"
#include "AABB.h"
#include "Sphere.h"


namespace CU
{
	Camera::Camera()
	//	: Camera(0.f, 0.f, 0.f, 0.f, 0.f)
	{
	}

	Camera::Camera(
		const float aFov,
		const float aProjectionWidth,
		const float aProjectionHeight,
		const float aNear,
		const float aFar)
	{
		Init(aFov, aProjectionWidth, aProjectionHeight, aNear, aFar);
	}

	Camera::~Camera()
	{
	}

	void Camera::Init(
		const float aFov,
		const float aProjectionWidth,
		const float aProjectionHeight,
		const float aNear,
		const float aFar)
	{
		myProjection = myProjection.CreateProjectionMatrixLH(aNear, aFar, aProjectionWidth , aProjectionHeight , (aFov) /** (3.14159265f / 180.f)*/);
		myProjectionInverse = myProjection.GetInverted();

		myFrustum.SetFrustum(aFar, aNear, aFov, aProjectionWidth, aProjectionHeight);
		myTransformation = Matrix44f::Identity;
		myFar = aFar;
		myNear = aNear;

		myWidth = aProjectionWidth;
		myHeight = aProjectionHeight;
	}
	//Orthographic, Width and Height does not care about pixelrino only ingame distances, 100  Width is 100 with ingame, if that is meters or whatever is up to game yo
	void Camera::Init(const float aProjectionWidth, const float aProjectionHeight, const float aNear, const float aFar)
	{
		myProjection = myProjection.CreateOrthogonalProjectionMatrixLH(aNear, aFar, aProjectionWidth, aProjectionHeight);
		myProjectionInverse = myProjection.GetInverted();

		myFrustum.SetFrustum(aFar, aNear, 0.0f, aProjectionWidth, aProjectionHeight);
		myTransformation = Matrix44f::Identity;
		myFar = aFar;
		myNear = aNear;

		myWidth = aProjectionWidth;
		myHeight = aProjectionHeight;

	}

	void Camera::ReInit(
		const float aFov,
		const float aProjectionWidth,
		const float aProjectionHeight,
		const float aNear,
		const float aFar)
	{
		myProjection = myProjection.CreateProjectionMatrixLH(aNear, aFar, aProjectionWidth, aProjectionHeight, aFov * (3.14159265f / 180.f));
		myProjectionInverse = myProjection.GetInverted();

		myFrustum.SetFrustum(aFar, aNear, aFov, aProjectionWidth, aProjectionHeight);
		myFar = aFar;
		myNear = aNear;

		myWidth = aProjectionWidth;
		myHeight = aProjectionHeight;
	}

	void Camera::ReInit(
		const float aProjectionWidth,
		const float aProjectionHeight,
		const float aNear,
		const float aFar)
	{
		myProjection = myProjection.CreateOrthogonalProjectionMatrixLH(aNear, aFar, aProjectionWidth, aProjectionHeight);
		myProjectionInverse = myProjection.GetInverted();

		myFrustum.SetFrustum(aFar, aNear, 0.0f, aProjectionWidth, aProjectionHeight);
		myFar = aFar;
		myNear = aNear;

		myWidth = aProjectionWidth;
		myHeight = aProjectionHeight;
	}

	Matrix44f Camera::GetInverse() const
	{
		Matrix44f tempMatrix = myTransformation;
		Vector4f translation(tempMatrix.GetPosition());
		tempMatrix.SetPosition(Vector3f(0.0f, 0.0f, 0.0f));
		translation *= -1;
		translation.w = 1;
		tempMatrix.Transpose();
		translation = translation * tempMatrix;
		tempMatrix.SetPosition(Vector3f(translation.x, translation.y, translation.z));
		return tempMatrix;
	}

	const Matrix44f& Camera::GetProjection() const
	{
		return myProjection;
	}

	const Matrix44f& Camera::GetProjectionInverse() const
	{
		return myProjectionInverse;
	}

	const Vector3f Camera::GetPosition() const
	{
		return myTransformation.GetPosition();
	}

	void Camera::Translate(Vector3f aVector)
	{
		Matrix44f translationMatrix;
		translationMatrix.SetPosition(aVector);

		myTransformation = translationMatrix * myTransformation;
	}
	void Camera::LookAt(const CU::Vector3f& aPosition)
	{
		myTransformation.LookAt(aPosition);
	}

	void Camera::Roll(const float aAngle)
	{
		myTransformation = myTransformation.CreateRotateAroundZ(aAngle) * myTransformation;
	}
	void Camera::Pitch(const float aAngle)
	{
		myTransformation = myTransformation.CreateRotateAroundX(aAngle) * myTransformation;

	}
	void Camera::Jaw(const float aAngle)
	{
		myTransformation = myTransformation.CreateRotateAroundY(aAngle) * myTransformation;
	}

	void Camera::TranslateForward(const float aAmount)
	{
		Vector3f position = myTransformation.GetPosition();
		Vector3f forwardVector(myTransformation.m31, myTransformation.m32, myTransformation.m33);
		position += forwardVector * aAmount;

		myTransformation.SetPosition(position);
	}

	void Camera::TranslateSideways(const float aAmount)
	{
		Vector3f position = myTransformation.GetPosition();
		Vector3f forwardVector(myTransformation.m11, myTransformation.m12, myTransformation.m13);
		position += forwardVector * aAmount;

		myTransformation.SetPosition(position);
	}
	void Camera::SetTransformation(const Matrix44f & aTransformation)
	{
		myTransformation = aTransformation;
	}
	const Matrix44f& Camera::GetTransformation() const
	{
		return myTransformation;
	}


	bool Camera::IsInside(const CU::Sphere& aSphere)
	{
		CU::Matrix44f inverse = GetInverse();
		CU::Sphere sphere = aSphere;
		sphere.myCenterPos = aSphere.myCenterPos * inverse;
		return myFrustum.IsInside(aSphere);
	}

	void Camera::SetPosition(const CU::Vector3f& aPosition)
	{
		myTransformation.SetPosition(aPosition);
	}

}