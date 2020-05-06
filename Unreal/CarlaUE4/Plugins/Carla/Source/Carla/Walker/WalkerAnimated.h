#pragma once

#include "GameFramework/Character.h"
#include "WalkerAnimated.generated.h"

UCLASS()
class AWalkerAnimator: public ACharacter
{
  GENERATED_BODY()

public:

  AWalkerAnimator(const FObjectInitializer &ObjectInitializer)
    : Super(ObjectInitializer)
  {
    PrimaryActorTick.bCanEverTick = true;
  }

  UFUNCTION(BlueprintImplementableEvent)
  void OnTriggerAnimation(int Animation);
    
};