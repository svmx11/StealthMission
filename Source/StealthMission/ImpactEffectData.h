#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NiagaraSystem.h"
#include "ImpactEffectData.generated.h"

USTRUCT(BlueprintType)
struct FImpactEffectData : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact")
    UNiagaraSystem* ImpactEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact")
    USoundBase* ImpactSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact")
    UMaterialInterface* DecalMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact")
    FVector DecalSize = FVector(5.f, 5.f, 5.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Impact")
    float DecalLifeSpan = 10.f;
};