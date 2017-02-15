#pragma once
#include "../CommonUtilities/vector2.h"

struct ID3D11ShaderResourceView;
class CDXFramework;
class CTextureManager;

class CTexture
{
public:
	CTexture();
	CTexture(const CTexture& aTexture) = delete;
	~CTexture();

	CTexture& operator=(const CTexture& aTexture);
	CTexture& operator=(CTexture&& aTexture);

	const CU::Vector2ui& GetSize() const;
	const unsigned int GetMipMapCount() const;
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11ShaderResourceView** GetShaderResourceViewPointer();

	inline void AddRef();
	inline int DecRef();

private:
	friend CTextureManager;
	CU::Vector2ui mySize;
	ID3D11ShaderResourceView* myTexture;
	unsigned int myMipMapCount;
	int myRefCount;
	
#ifdef _DEBUG
	std::wstring myPath;
#endif
};

inline const CU::Vector2ui & CTexture::GetSize() const
{
	return mySize;
}

inline const unsigned int CTexture::GetMipMapCount() const
{
	return myMipMapCount;
}

inline ID3D11ShaderResourceView* CTexture::GetShaderResourceView()
{
	return myTexture;
}

inline ID3D11ShaderResourceView ** CTexture::GetShaderResourceViewPointer()
{
	return &myTexture;
}

inline void CTexture::AddRef()
{
	++myRefCount;
}

inline int CTexture::DecRef()
{
	return (--myRefCount);
}


