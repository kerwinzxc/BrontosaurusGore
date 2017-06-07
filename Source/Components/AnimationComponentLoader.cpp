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
	void LoadMoving(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
	void LoadShoot(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
	void LoadEquip(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
	void LoadUnequip(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
	void LoadJump(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
	void LoadRangedAttack(const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);


	void LoadSingleAnimation(const std::string& aType, const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aMap);

	void LoadAnimations(const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		{
			const std::string& filePath = aModelComponent.GetFilePath();
			if (filePath.find("M_ShotgunPlayerAnim") != std::string::npos)
			{
				int br = 0;
			}
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

			if (statesObject.HasKey("moving"))
			{
				CU::CJsonValue moveingObject = statesObject["moving"];
				LoadMoving(moveingObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("shoot"))
			{
				CU::CJsonValue shootObject = statesObject["shoot"];
				LoadShoot(shootObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("equip"))
			{
				CU::CJsonValue equipObject = statesObject["equip"];
				LoadEquip(equipObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("unequip"))
			{
				CU::CJsonValue unequipObject = statesObject["unequip"];
				LoadUnequip(unequipObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("jump"))
			{
				CU::CJsonValue jumpObject = statesObject["jump"];
				LoadJump(jumpObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("ranged"))
			{
				CU::CJsonValue rangedObject = statesObject["ranged"];
				LoadRangedAttack(rangedObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("die"))
			{
				CU::CJsonValue rangedObject = statesObject["die"];
				LoadSingleAnimation("die",rangedObject, aModelComponent, aAnimationStates);
			}

			if (statesObject.HasKey("melee"))
			{
				CU::CJsonValue rangedObject = statesObject["melee"];
				LoadSingleAnimation("melee", rangedObject, aModelComponent, aAnimationStates);
			}
			if (statesObject.HasKey("beginfly"))
			{
				CU::CJsonValue rangedObject = statesObject["beginfly"];
				LoadSingleAnimation("beginfly", rangedObject, aModelComponent, aAnimationStates);
			}
			if (statesObject.HasKey("endfly"))
			{
				CU::CJsonValue rangedObject = statesObject["endfly"];
				LoadSingleAnimation("endfly", rangedObject, aModelComponent, aAnimationStates);
			}
			if (statesObject.HasKey("flyattack"))
			{
				CU::CJsonValue rangedObject = statesObject["flyattack"];
				LoadSingleAnimation("flyattack", rangedObject, aModelComponent, aAnimationStates);
			}
			if (statesObject.HasKey("chargeStart"))
			{
				CU::CJsonValue rangedObject = statesObject["chargeStart"];
				LoadSingleAnimation("chargeStart", rangedObject, aModelComponent, aAnimationStates);
			}
			if (statesObject.HasKey("chargeRun"))
			{
				CU::CJsonValue rangedObject = statesObject["chargeRun"];
				LoadSingleAnimation("chargeRun", rangedObject, aModelComponent, aAnimationStates);
			}
		}
	}

	void LoadMoving(const CU::CJsonValue& aMovingObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation movingState;
		int animationKey = SAnimationState::AnimationStates.Find(aMovingObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			movingState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aMovingObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				movingState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				movingState.myNextAnimationKey = eAnimationState::none;
			}

			movingState.myIsLooping = aMovingObject["loop"].GetBool();
			if (movingState.myIsLooping)
			{
				if (aMovingObject.HasKey("coolDownTime"))
				{
					movingState.myCoolDownTime = aMovingObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				movingState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::walk01);
			}


			aAnimationStates["moving"] = movingState;
		}
	}

	void LoadShoot(const CU::CJsonValue& aShootObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation shootState;
		int animationKey = SAnimationState::AnimationStates.Find(aShootObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			shootState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aShootObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				shootState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				shootState.myNextAnimationKey = eAnimationState::none;
			}

			shootState.myIsLooping = aShootObject["loop"].GetBool();
			if (shootState.myIsLooping)
			{
				if (aShootObject.HasKey("coolDownTime"))
				{
					shootState.myCoolDownTime = aShootObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				shootState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::shot01);
			}


			aAnimationStates["shoot"] = shootState;
		}
	}

	void LoadEquip(const CU::CJsonValue& aEquipObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation equipState;
		int animationKey = SAnimationState::AnimationStates.Find(aEquipObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			equipState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aEquipObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				equipState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				equipState.myNextAnimationKey = eAnimationState::none;
			}

			equipState.myIsLooping = aEquipObject["loop"].GetBool();
			if (equipState.myIsLooping)
			{
				if (aEquipObject.HasKey("coolDownTime"))
				{
					equipState.myCoolDownTime = aEquipObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				equipState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::equip01);
			}


			aAnimationStates["equip"] = equipState;
		}
	}

	void LoadUnequip(const CU::CJsonValue& aUnequipObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation equipState;
		int animationKey = SAnimationState::AnimationStates.Find(aUnequipObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			equipState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aUnequipObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				equipState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				equipState.myNextAnimationKey = eAnimationState::none;
			}

			equipState.myIsLooping = aUnequipObject["loop"].GetBool();
			if (equipState.myIsLooping)
			{
				if (aUnequipObject.HasKey("coolDownTime"))
				{
					equipState.myCoolDownTime = aUnequipObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				equipState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::equip01);
			}


			aAnimationStates["unequip"] = equipState;
		}
	}

	void LoadJump(const CU::CJsonValue& aJumpObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation jumpState;
		int animationKey = SAnimationState::AnimationStates.Find(aJumpObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			jumpState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aJumpObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				jumpState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				jumpState.myNextAnimationKey = eAnimationState::none;
			}

			jumpState.myIsLooping = aJumpObject["loop"].GetBool();
			if (jumpState.myIsLooping)
			{
				if (aJumpObject.HasKey("coolDownTime"))
				{
					jumpState.myCoolDownTime = aJumpObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				jumpState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::jump01);
			}


			aAnimationStates["jump"] = jumpState;
		}
	}

	void LoadRangedAttack(const CU::CJsonValue& aRangedAttackObject, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates)
	{
		SAnimation rangedAttackState;
		int animationKey = SAnimationState::AnimationStates.Find(aRangedAttackObject["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			rangedAttackState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aRangedAttackObject["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				rangedAttackState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				rangedAttackState.myNextAnimationKey = eAnimationState::none;
			}

			rangedAttackState.myIsLooping = aRangedAttackObject["loop"].GetBool();
			if (rangedAttackState.myIsLooping)
			{
				if (aRangedAttackObject.HasKey("coolDownTime"))
				{
					rangedAttackState.myCoolDownTime = aRangedAttackObject["coolDownTime"].GetFloat();
				}
			}
			else
			{
				rangedAttackState.myCoolDownTime = aModelComponent.GetAnimationDuration(eAnimationState::throwAttack01);
			}


			aAnimationStates["ranged"] = rangedAttackState;
		}
	}

	void LoadSingleAnimation(const std::string& aType, const CU::CJsonValue& aJsonValue, const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aMap)
	{
		SAnimation rangedAttackState;
		int animationKey = SAnimationState::AnimationStates.Find(aJsonValue["animation"].GetString());
		if (animationKey != SAnimationState::AnimationStates.FoundNone)
		{
			rangedAttackState.myAnimationKey = static_cast<eAnimationState>(animationKey);
			int nextAnimationKey = SAnimationState::AnimationStates.Find(aJsonValue["nextAnimation"].GetString());
			if (nextAnimationKey != SAnimationState::AnimationStates.FoundNone)
			{
				rangedAttackState.myNextAnimationKey = static_cast<eAnimationState>(nextAnimationKey);
			}
			else
			{
				rangedAttackState.myNextAnimationKey = eAnimationState::none;
			}

			rangedAttackState.myIsLooping = aJsonValue["loop"].GetBool();
			if (rangedAttackState.myIsLooping)
			{
				if (aJsonValue.HasKey("coolDownTime"))
				{
					rangedAttackState.myCoolDownTime = aJsonValue["coolDownTime"].GetFloat();
				}
			}
			else
			{
				int animation = SAnimationState::AnimationStates.Find(aJsonValue["animation"].GetString());
				rangedAttackState.myCoolDownTime = aModelComponent.GetAnimationDuration(static_cast<eAnimationState>(animation));
			}


			aMap[aType] = rangedAttackState;
		}
	}
}
