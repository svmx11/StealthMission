// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AmmoAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHMISSION_API UAmmoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly) FGameplayAttributeData AmmoInClip;
    UPROPERTY(BlueprintReadOnly) FGameplayAttributeData MaxAmmoInClip;
    UPROPERTY(BlueprintReadOnly) FGameplayAttributeData ReserveAmmo;
};
