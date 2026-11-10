// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UHealthAttributeSet::UHealthAttributeSet()
{
	Health = FGameplayAttributeData(100.0f);
	MaxHealth = FGameplayAttributeData(100.0f);
}

FGameplayTag UHealthAttributeSet::ExtractDamageType(const FGameplayEffectSpec& Spec)
{
	static const FGameplayTag DamageRoot = FGameplayTag::RequestGameplayTag(TEXT("Damage"));

	FGameplayTagContainer AllTags;
	Spec.GetAllAssetTags(AllTags); // теги GE + добавленные через Add Asset Tag
	if (const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags())
	{
		AllTags.AppendTags(*SourceTags); // сюда пишет AGASCharacterBase::ApplyDamage
	}

	const FGameplayTagContainer DamageTypes = AllTags.Filter(FGameplayTagContainer(DamageRoot));
	return DamageTypes.Num() > 0 ? DamageTypes.First() : FGameplayTag();
}

void UHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void UHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamage = GetDamage();
		SetDamage(0.0f); // meta-атрибут не хранит значение между применениями

		if (LocalDamage > 0.0f && !bIsDead)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			const FGameplayTag DamageType = ExtractDamageType(Data.EffectSpec);
			AActor* DamageInstigator = Data.EffectSpec.GetContext().GetOriginalInstigator();

			UE_LOG(LogTemp, Verbose, TEXT("%s took %.1f damage (%s)"),
				*GetNameSafe(GetOwningActor()), LocalDamage, *DamageType.ToString());

			const float Percent = GetMaxHealth() > 0.0f ? NewHealth / GetMaxHealth() : 0.0f;
			OnHealthChanged.Broadcast(Percent);
			OnDamageTaken.Broadcast(LocalDamage, DamageInstigator, DamageType);

			if (NewHealth <= 0.0f)
			{
				bIsDead = true;
				OnDeath.Broadcast(DamageType);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Лечение/дебаг меняют Health напрямую — только пересчитать процент
		const float Percent = GetMaxHealth() > 0.0f ? GetHealth() / GetMaxHealth() : 0.0f;
		OnHealthChanged.Broadcast(Percent);
	}
}

void UHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, Health, OldHealth);
}

void UHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UHealthAttributeSet, MaxHealth, OldMaxHealth);
}

void UHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}