#include "Carla.h"
#include "Carla/Sensor/ActorIdSensor.h"
#include "Carla/Actor/ActorBlueprintFunctionLibrary.h"
#include "Carla/Game/CarlaEpisode.h"
#include "Carla/Util/BoundingBoxCalculator.h"
#include "Components/BoxComponent.h"
#include "Carla/Vehicle/CarlaWheeledVehicle.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/CollisionProfile.h"
#include "DrawDebugHelpers.h"

#include <compiler/disable-ue4-macros.h>
#include "carla/geom/Math.h"
#include <compiler/enable-ue4-macros.h>

TArray<FVector> GetEdgesFromBoundingBox(FBoundingBox* Box, FVector Target);

AIdentifierSensor::AIdentifierSensor(const FObjectInitializer &ObjectInitializer)
  : Super(ObjectInitializer)
{
  PrimaryActorTick.bCanEverTick = true;
}

FActorDefinition AIdentifierSensor::GetSensorDefinition()
{
  auto Definition = UActorBlueprintFunctionLibrary::MakeGenericSensorDefinition(
    TEXT("other"),
    TEXT("actor_identification"));

  FActorVariation FOV;
  FOV.Id = TEXT("fov");
  FOV.Type = EActorAttributeType::Float;
  FOV.RecommendedValues = { TEXT("30") };
  FOV.bRestrictToRecommended = false;

  FActorVariation InitConeRadius;
  InitConeRadius.Id = TEXT("init_cone_radius");
  InitConeRadius.Type = EActorAttributeType::Float;
  InitConeRadius.RecommendedValues = { TEXT("1") };
  InitConeRadius.bRestrictToRecommended = false;

  FActorVariation Range;
  Range.Id = TEXT("range");
  Range.Type = EActorAttributeType::Float;
  Range.RecommendedValues = { TEXT("100") };
  Range.bRestrictToRecommended = false;

  Definition.Variations.Append({ FOV, InitConeRadius, Range });
  return Definition;
}

void AIdentifierSensor::Set(const FActorDescription &Description)
{
  Super::Set(Description);

  SetFOV(UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
    "FOV",
    Description.Variations,
    60.0f));

  SetInitConeRadius(UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
    "InitConeRadius",
    Description.Variations,
    1.0f));

  SetRange(UActorBlueprintFunctionLibrary::RetrieveActorAttributeToFloat(
    "Range",
    Description.Variations,
    100.0f));
}

void AIdentifierSensor::SetRange(float NewRange)
{
  Range = NewRange;
}

void AIdentifierSensor::SetFOV(float NewFOV)
{
  FOV = NewFOV;
}

void AIdentifierSensor::SetInitConeRadius(float NewInitConeRadius)
{
  InitConeRadius = NewInitConeRadius;
}

void AIdentifierSensor::Tick(const float DeltaTime)
{
  Super::Tick(DeltaTime);

  TArray<AActor *> VisibleActors;
  for( TObjectIterator<AActor> Itr; Itr; ++Itr )
  {
      if(checkVisibility(*Itr)){
          VisibleActors.Add(*Itr);
      }
  }
  VisibleActors.Remove(GetOwner());

  auto DataStream = GetDataStream(*this);
  DataStream.Send(*this, GetEpisode(), VisibleActors);
}

bool AIdentifierSensor::checkVisibility(AActor * actor)
{
  //Only check vehicles or walkers
  if ((Cast<ACarlaWheeledVehicle>(actor) == nullptr && Cast<ACharacter>(actor) == nullptr) || actor==this->GetOwner()){return false;}
  
  bool Visible = false;
  FVector SensorLocation = this->GetActorLocation();
  FVector ForwardVector = this->GetActorForwardVector();

  FVector Target = actor->GetActorLocation();
  FBoundingBox BoundingBox = UBoundingBoxCalculator::GetActorBoundingBox(actor);
  DrawDebugBox(GetWorld(), Target, BoundingBox.Extent, FColor::Green, false, 0.1f, ECC_WorldStatic, 10.f);
  
  const float TO_CM = 100.0f;
  float perOut;
  float initRadius = 1.0f;

  //For all point in bounding box
  for(auto& target : GetEdgesFromBoundingBox(&BoundingBox, Target))
  {
    const bool InCone = FMath::GetDistanceWithinConeSegment(
        target,
        SensorLocation,
        ForwardVector * Range * TO_CM,
        InitConeRadius * TO_CM,
        (InitConeRadius + Range * FMath::Tan(FOV*PI/180.0f)) * TO_CM,
        perOut
    );
    if(!InCone){return false;}


    TraceParams = FCollisionQueryParams(FName(TEXT("Laser_Trace")), true, this);
    TraceParams.AddIgnoredActor(this->GetOwner());
    TraceParams.bTraceComplex = true;
    TraceParams.bReturnPhysicalMaterial = false;

    FHitResult OutHit(ForceInit);
    //   //If hit before actor, not visible
    GetWorld()->LineTraceSingleByChannel(
        OutHit,
        SensorLocation,
        target,
        ECC_MAX,
        TraceParams,
        FCollisionResponseParams::DefaultResponseParam
    );
    Visible = Visible || !OutHit.bBlockingHit || FVector::Distance(OutHit.ImpactPoint, target) < 100.f;

    if(OutHit.bBlockingHit)
    {
        DrawDebugLine(GetWorld(), SensorLocation, OutHit.ImpactPoint, FColor::Green, false, 5.f, ECC_WorldStatic, 5.f);
        DrawDebugLine(GetWorld(), OutHit.ImpactPoint, target, FColor::Red, false, 5.f, ECC_WorldStatic, 5.f);
        DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 10, FColor::Red, false, 5.f); 
    }
  }
  return Visible;
}

TArray<FVector> GetEdgesFromBoundingBox(FBoundingBox* Box, FVector Target)
{
  const FVector Location = Box->Origin;
  const FVector Extent = Box->Extent;

  TArray<FVector> Out{
      FVector{Target.X + Location.X - Extent.X/2.f, Target.Y + Location.Y - Extent.Y/2.f, Target.Z + Location.Z - Extent.Z/2.f},
      FVector{Target.X + Location.X - Extent.X/2.f, Target.Y + Location.Y - Extent.Y/2.f, Target.Z + Location.Z + Extent.Z/2.f},
      FVector{Target.X + Location.X - Extent.X/2.f, Target.Y + Location.Y + Extent.Y/2.f, Target.Z + Location.Z - Extent.Z/2.f},
      FVector{Target.X + Location.X - Extent.X/2.f, Target.Y + Location.Y + Extent.Y/2.f, Target.Z + Location.Z + Extent.Z/2.f},
      FVector{Target.X + Location.X + Extent.X/2.f, Target.Y + Location.Y - Extent.Y/2.f, Target.Z + Location.Z - Extent.Z/2.f},
      FVector{Target.X + Location.X + Extent.X/2.f, Target.Y + Location.Y - Extent.Y/2.f, Target.Z + Location.Z + Extent.Z/2.f},
      FVector{Target.X + Location.X + Extent.X/2.f, Target.Y + Location.Y + Extent.Y/2.f, Target.Z + Location.Z - Extent.Z/2.f},
      FVector{Target.X + Location.X + Extent.X/2.f, Target.Y + Location.Y + Extent.Y/2.f, Target.Z + Location.Z + Extent.Z/2.f},
  };
  return Out;
};


