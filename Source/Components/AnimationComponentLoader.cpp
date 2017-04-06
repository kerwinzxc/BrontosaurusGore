#include "stdafx.h"
#include "AnimationComponentLoader.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"

#include "../CommonUtilities/JsonValue.h"

#include "../TShared/AnimationState.h"
DECLARE_ANIMATION_ENUM_AND_STRINGS;

//struct SAnimation
//{
//	eAnimationState myAnimationKey;
//	eAnimationState myNextAnimationKey;
//	float myAnimationBlend = 0.f;
//	float myBlendTime = 0.f;
//	float myLifeTime = FLT_MAX;
//	bool myIsLooping = true;
//};

namespace AnimationComponentLoader
{
	static SAnimation locIdleState;

	void LoadAnimations(const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		//CU::GrowingArray<eAnimationState> animationStates;
		//if (aModelComponent.GetAnimationStates(animationStates))
		{
			const std::string& filePath = aModelComponent.GetFilePath();
			std::string jsonPath = filePath;
			if (!CU::FindAndReplace(jsonPath, "Models/Meshes", "Json/Animations"))
			{
				CU::FindAndReplace(jsonPath, "Models", "Json");
			}
			jsonPath -= "fbx";
			jsonPath += "json";

			if (!CU::CJsonValue::FileExists(jsonPath))
			{
				DL_PRINT_WARNING("File does not exist %s", jsonPath.c_str());
				return;
			}

			CU::CJsonValue rootObject(jsonPath);
			if (!rootObject.HasKey("states"))
			{
				return;
			}
			CU::CJsonValue statesObject = rootObject["states"];

			//if (statesObject.HasKey("idle"))
			//{
			//	CU::CJsonValue idleObject = statesObject["idle"];
			//	SAnimation idleState;

			//}

			if (statesObject.HasKey("shoot"))
			{
				CU::CJsonValue shootObject = statesObject["shoot"];
				SAnimation shootState;
				int animationKey = SAnimationState::AnimationStates.Find(shootObject["animation"].GetString());
				if (animationKey != SAnimationState::AnimationStates.FoundNone)
				{
					shootState.myAnimationKey = static_cast<eAnimationState>(animationKey);
					int nextAnimationKey = SAnimationState::AnimationStates.Find(shootObject["nextAnimation"].GetString());
					if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
					{
						shootState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
					}
					else
					{
						shootState.myNextAnimationKey = eAnimationState::none;
					}
					
					shootState.myIsLooping = shootObject["loop"].GetBool();
					if (shootState.myIsLooping)
					{
						if (shootObject.HasKey("coolDownTime"))
						{
							shootState.myCoolDownTime = shootObject["coolDownTime"].GetFloat();
						}
					}
					else
					{
						shootState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::shot01);
					}


					aAnimationStates["shoot"] = shootState;
				}
			}
			else
			{
				DL_PRINT_WARNING("File does not have key shoot %s", jsonPath.c_str());
			}

			//for (eAnimationState animationState : animationStates)
			//{
			//	
			//}
		}
	}
}
