#pragma once

namespace CU
{
	class DynamicString;
}

class CTexture;
class CEffect;

struct SCharacterInfo
{
	int id;
	int x;
	int y;
	int width;
	int height;
	int xOffset;
	int yOffset;
	int xAdvance;
};

class CFont
{
public:
	CFont(const std::string& aFontPath);
	~CFont();

	inline const SCharacterInfo& GetCharacterInfo(const char aCharacter) const;
	inline CTexture* GetTexture();
	inline CEffect* GetEffect();
	inline float GetLineHeight() const;

	inline void AddRef();
	inline unsigned int DecRef();

private:
	void LoadFont(const std::string& aFontPath);
	void LoadTexture(const char* aFontPath);
	void CreateEffect();

	CU::GrowingArray<SCharacterInfo, char> myCharacters;
	CTexture* myCharacterAtlas;
	CEffect* myEffect;
	float myLineHeight;
	
	int myRefCount;
};

const SCharacterInfo& CFont::GetCharacterInfo(const char aCharacter) const
{
	return myCharacters[aCharacter];
}

inline CTexture* CFont::GetTexture()
{
	return myCharacterAtlas;
}

inline CEffect* CFont::GetEffect()
{
	return myEffect;
}

inline float CFont::GetLineHeight() const
{
	return myLineHeight;
}

inline void CFont::AddRef()
{
	++myRefCount;
}

const unsigned int KILL_ME = 0;
const unsigned int KEEP_ME = 1;

inline unsigned int CFont::DecRef()
{
	--myRefCount;

	if (myRefCount <= 0)
	{
		return KILL_ME;
	}

	return KEEP_ME;
}
