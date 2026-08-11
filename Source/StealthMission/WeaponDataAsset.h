// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class STEALTHMISSION_API UWeaponDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    // Тег текущего оружия — единая точка правды для анимации, GAS-способностей, UI
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FGameplayTag WeaponTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
    FText DisplayName;

    // Статик-меш подходит для пистолета без анимируемых частей (затвор и т.д. — позже Skeletal Mesh)
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    TObjectPtr<UStaticMesh> WeaponMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    FName AttachSocketName;

    // Класс слоя анимации — то, что раньше выбиралось через Switch on EWeaponType
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    TSubclassOf<UAnimInstance> AnimLayerClass;

    // Задел под раздел 17.7 — способности оружия, пока не используются
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<class UGameplayAbility> FireAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<class UGameplayAbility> ReloadAbility;
};
