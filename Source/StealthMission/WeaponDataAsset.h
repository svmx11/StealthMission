// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "NiagaraSystem.h"
#include "WeaponDataAsset.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EFireMode : uint8
{
    Single,
    Automatic,
    Burst
};

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

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    FName MuzzleSocketName; // для трейса/спавна эффекта выстрела

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    UNiagaraSystem* MuzzleFlashEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visuals")
    UNiagaraSystem* BulletTracerEffect;

    // Класс слоя анимации — то, что раньше выбиралось через Switch on EWeaponType
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    TSubclassOf<UAnimInstance> AnimLayerClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* FireMontage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* ReloadMontage; // длительность = длительность монтажа

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    EFireMode FireMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float RateOfFire; // выстрелов в секунду, либо RPM

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float BaseDamage;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
    int32 MagazineSize;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ammo")
    int32 MaxReserveAmmo;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
    USoundBase* FireSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
    USoundBase* EmptyMagazineSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
    USoundBase* ReloadSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float Range = 5000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float HipFireSpreadAngle = 5.0f;   // градусы конуса разброса от бедра

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float AimSpreadAngle = 0.2f;       // почти точный выстрел при прицеливании

    // Задел под раздел 17.7 — способности оружия, пока не используются
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<class UGameplayAbility> FireAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<class UGameplayAbility> AimAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Abilities")
    TSubclassOf<class UGameplayAbility> ReloadAbility;
};
