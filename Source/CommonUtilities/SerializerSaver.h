#pragma once
#include "Serializer.h"

namespace CU
{
	class CSerilizerSaver : public ISerializer
	{
	public:
		CSerilizerSaver(const unsigned int aBufferStartSize = 32);
		~CSerilizerSaver();

		virtual void Cerealize(unsigned char& aChar) override;
		virtual void Cerealize(unsigned int& aUInt) override;
		virtual void Cerealize(float& aFloat) override;
		virtual void Cerealize(double& aDouble) override;
		virtual void Cerealize(CU::Vector2f& aVector2f) override;
		virtual void Cerealize(CU::Vector3f& aVector3f) override;
		virtual void Cerealize(CU::Vector4f& aVector4f) override;
		virtual void Cerealize(CU::Matrix33f& aMatrix33f) override;
		virtual void Cerealize(CU::Matrix44f& aMatrix44f) override;
		virtual void Cerealize(std::string& aString) override;

		const CU::GrowingArray<char>& GetBuffer() const;

	private:
		CU::GrowingArray<char> myBuffer;
	};
}
