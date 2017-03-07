#include "stdafx.h"
#include "LoadInputComponent.h"
#include "../Components/InputComponent.h"
#include "../Components/ComponentManager.h"

int LoadInputComponent(KLoader::SLoadedComponentData someData)
{

	CInputComponent* inputComponent = new CInputComponent();

	CComponentManager::GetInstance().RegisterComponent(inputComponent);
	return inputComponent->GetId();
}
