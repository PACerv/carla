#pragma once
#include "Carla/Sensor/Sensor.h"

#include "Carla/Actor/ActorDefinition.h"
#include "Carla/Actor/ActorDescription.h"

#include "ActorIdSensor.generated.h"

UCLASS()
class CARLA_API AIdentifierSensor : public ASensor
{
  GENERATED_BODY()

  public:

  AIdentifierSensor(const FObjectInitializer &ObjectInitializer);

  static FActorDefinition GetSensorDefinition();

  void Set(const FActorDescription &ActorDescription) override;

  UFUNCTION(BlueprintCallable, Category = "Identification")
  void SetRange(float NewRange);

  UFUNCTION(BlueprintCallable, Category = "Identification")
  void SetFOV(float NewFOV);

  UFUNCTION(BlueprintCallable, Category = "Identification")
  void SetInitConeRadius(float NewInitConeRadius);

protected:
  virtual void Tick(float DeltaTime) override;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection")
  float Range;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection")
  float FOV;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Detection")
  float InitConeRadius;

private:
  bool checkVisibility(AActor* actor);

  FCollisionQueryParams TraceParams;

  UWorld* World;
};