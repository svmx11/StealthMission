// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponDataAsset.h"
#include "WeaponManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChanged, UWeaponDataAsset*, NewWeapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STEALTHMISSION_API UWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UWeaponManagerComponent();

    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FOnWeaponChanged OnWeaponChanged;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipWeapon(UWeaponDataAsset* NewWeaponData);

    UFUNCTION(BlueprintPure, Category = "Weapon")
    UWeaponDataAsset* GetCurrentWeaponData() const { return CurrentWeaponData; }

    UFUNCTION(BlueprintPure, Category = "Weapon")
    bool IsArmed() const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    TObjectPtr<UWeaponDataAsset> DefaultWeaponData;

    UPROPERTY()
    TObjectPtr<UWeaponDataAsset> CurrentWeaponData;

    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> EquippedMeshComponent;

    void AttachWeaponMesh(UWeaponDataAsset* WeaponData);
    void UpdateWeaponTag(UWeaponDataAsset* OldWeapon, UWeaponDataAsset* NewWeapon);	
};
