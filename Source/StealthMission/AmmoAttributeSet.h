// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AmmoAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoValueChanged, float, OldValue, float, NewValue);

/**
 * 
 */
UCLASS()
class STEALTHMISSION_API UAmmoAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
    UAmmoAttributeSet();

    // Текущее число патронов в обойме
    UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_AmmoInClip)
    FGameplayAttributeData AmmoInClip;
    ATTRIBUTE_ACCESSORS(UAmmoAttributeSet, AmmoInClip)

    // Вместимость текущей обоймы (зависит от оружия, ставится при экипировке)
    UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_MaxAmmoInClip)
    FGameplayAttributeData MaxAmmoInClip;
    ATTRIBUTE_ACCESSORS(UAmmoAttributeSet, MaxAmmoInClip)

    // Запасные патроны (не в обойме)
    UPROPERTY(BlueprintReadOnly, Category = "Ammo", ReplicatedUsing = OnRep_ReserveAmmo)
    FGameplayAttributeData ReserveAmmo;
    ATTRIBUTE_ACCESSORS(UAmmoAttributeSet, ReserveAmmo)

    UFUNCTION(BlueprintPure, Category = "Ammo")
    static float GetAmmoInClipValue(UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintPure, Category = "Ammo")
    static float GetMaxAmmoInClipValue(UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintPure, Category = "Ammo")
    static float GetReserveAmmoValue(UAbilitySystemComponent* ASC);

    UFUNCTION(BlueprintPure, Category = "Ammo")
    static FGameplayAttribute GetAmmoInClipAttributeStatic() { return GetAmmoInClipAttribute(); }

    UFUNCTION(BlueprintPure, Category = "Ammo")
    static FGameplayAttribute GetReserveAmmoAttributeStatic() { return GetReserveAmmoAttribute(); }

    UPROPERTY(BlueprintAssignable, Category = "Ammo")
    FOnAmmoValueChanged OnAmmoInClipChanged;

    UPROPERTY(BlueprintAssignable, Category = "Ammo")
    FOnAmmoValueChanged OnReserveAmmoChanged;

    // Геттер инстанса набора атрибутов — без него из BP не достать сам AmmoAttributeSet
    UFUNCTION(BlueprintPure, Category = "Ammo")
    static UAmmoAttributeSet* GetAmmoAttributeSet(UAbilitySystemComponent* ASC);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
    UFUNCTION()
    virtual void OnRep_AmmoInClip(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    virtual void OnRep_MaxAmmoInClip(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    virtual void OnRep_ReserveAmmo(const FGameplayAttributeData& OldValue);
};
