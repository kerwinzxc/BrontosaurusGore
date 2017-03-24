#include "stdafx.h"
#include "SerializerSaver.h"

namespace CU
{
	CSerilizerSaver::CSerilizerSaver(const unsigned int aBufferStartSize)
		: myBuffer(aBufferStartSize)
	{
		assert(myBuffer.Capacity() > 0 && "incoming heap corruption");
	}

	CSerilizerSaver::~CSerilizerSaver()
	{
	}

	void CSerilizerSaver::Cerealize(unsigned char& aChar)
	{
		myBuffer.Add(aChar);
	}

	void CSerilizerSaver::Cerealize(unsigned int& aUInt)
	{
		myBuffer.AddChunk(&aUInt, sizeof(unsigned int));
	}

	void CSerilizerSaver::Cerealize(float& aFloat)
	{
		myBuffer.AddChunk(&aFloat, sizeof(float));
	}

	void CSerilizerSaver::Cerealize(double& aDouble)
	{
		myBuffer.AddChunk(&aDouble, sizeof(aDouble));
	}

	void CSerilizerSaver::Cerealize(CU::Vector2f& aVector2f)
	{
		myBuffer.AddChunk(&aVector2f, sizeof(CU::Vector2f));
	}

	void CSerilizerSaver::Cerealize(CU::Vector3f& aVector3f)
	{
		myBuffer.AddChunk(&aVector3f, sizeof(CU::Vector3f));
	}

	void CSerilizerSaver::Cerealize(CU::Vector4f& aVector4f)
	{
		myBuffer.AddChunk(&aVector4f, sizeof(CU::Vector4f));
	}

	void CSerilizerSaver::Cerealize(CU::Matrix33f& aMatrix33f)
	{
		myBuffer.AddChunk(&aMatrix33f, sizeof(CU::Matrix33f));
	}

	void CSerilizerSaver::Cerealize(CU::Matrix44f& aMatrix44f)
	{
		myBuffer.AddChunk(&aMatrix44f, sizeof(CU::Matrix44f));
	}

	void CSerilizerSaver::Cerealize(std::string& aString)
	{
		unsigned int stringLength = static_cast<unsigned int>(aString.size());
		Cerealize(stringLength);
		myBuffer.AddChunk(aString.c_str(), stringLength);
	}

	const CU::GrowingArray<char>& CSerilizerSaver::GetBuffer() const
	{
		return myBuffer;
	}
}
