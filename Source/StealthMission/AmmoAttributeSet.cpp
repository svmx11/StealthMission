// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UAmmoAttributeSet::UAmmoAttributeSet()
{
    InitAmmoInClip(0.f);
    InitMaxAmmoInClip(0.f);
    InitReserveAmmo(0.f);
}

float UAmmoAttributeSet::GetAmmoInClipValue(UAbilitySystemComponent* ASC)
{
    if (!ASC) return 0.f;
    const UAmmoAttributeSet* Set = ASC->GetSet<UAmmoAttributeSet>();
    return Set ? Set->GetAmmoInClip() : 0.f;
}

float UAmmoAttributeSet::GetMaxAmmoInClipValue(UAbilitySystemComponent* ASC)
{
    if (!ASC) return 0.f;
    const UAmmoAttributeSet* Set = ASC->GetSet<UAmmoAttributeSet>();
    return Set ? Set->GetMaxAmmoInClip() : 0.f;
}

float UAmmoAttributeSet::GetReserveAmmoValue(UAbilitySystemComponent* ASC)
{
    if (!ASC) return 0.f;
    const UAmmoAttributeSet* Set = ASC->GetSet<UAmmoAttributeSet>();
    return Set ? Set->GetReserveAmmo() : 0.f;
}

UAmmoAttributeSet* UAmmoAttributeSet::GetAmmoAttributeSet(UAbilitySystemComponent* ASC)
{
    return ASC ? const_cast<UAmmoAttributeSet*>(ASC->GetSet<UAmmoAttributeSet>()) : nullptr;
}

void UAmmoAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION_NOTIFY(UAmmoAttributeSet, AmmoInClip, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAmmoAttributeSet, MaxAmmoInClip, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UAmmoAttributeSet, ReserveAmmo, COND_None, REPNOTIFY_Always);
}

void UAmmoAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    if (Attribute == GetAmmoInClipAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmoInClip());
    }
    else if (Attribute == GetMaxAmmoInClipAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.f);
    }
    else if (Attribute == GetReserveAmmoAttribute())
    {
        NewValue = FMath::Max(NewValue, 0.f);
    }
}

void UAmmoAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Если MaxAmmoInClip уменьшился (например, при смене оружия на менее ёмкое) —
    // подрезать текущий AmmoInClip, чтобы не превышал новый максимум.
    if (Data.EvaluatedData.Attribute == GetMaxAmmoInClipAttribute())
    {
        if (GetAmmoInClip() > GetMaxAmmoInClip())
        {
            SetAmmoInClip(GetMaxAmmoInClip());
        }
    }

    if (Data.EvaluatedData.Attribute == GetAmmoInClipAttribute())
    {
        float OldValue = GetAmmoInClip();
        SetAmmoInClip(FMath::Clamp(GetAmmoInClip(), 0.f, GetMaxAmmoInClip()));
        OnAmmoInClipChanged.Broadcast(OldValue, GetAmmoInClip());
    }

    if (Data.EvaluatedData.Attribute == GetReserveAmmoAttribute())
    {
        float OldValue = GetReserveAmmo();
        SetReserveAmmo(FMath::Max(GetReserveAmmo(), 0.f));
        OnReserveAmmoChanged.Broadcast(OldValue, GetReserveAmmo());
    }
}

void UAmmoAttributeSet::OnRep_AmmoInClip(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAmmoAttributeSet, AmmoInClip, OldValue);
    OnAmmoInClipChanged.Broadcast(OldValue.GetCurrentValue(), AmmoInClip.GetCurrentValue());
}

void UAmmoAttributeSet::OnRep_MaxAmmoInClip(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAmmoAttributeSet, MaxAmmoInClip, OldValue);
}

void UAmmoAttributeSet::OnRep_ReserveAmmo(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAmmoAttributeSet, ReserveAmmo, OldValue);
    OnReserveAmmoChanged.Broadcast(OldValue.GetCurrentValue(), ReserveAmmo.GetCurrentValue());
}
