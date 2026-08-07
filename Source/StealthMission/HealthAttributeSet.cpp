// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UHealthAttributeSet::UHealthAttributeSet()
{
	Health = FGameplayAttributeData(100.0f);
	MaxHealth = FGameplayAttributeData(100.0f);
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
		SetDamage(0.0f); // обнуляем meta-атрибут сразу, он не должен ничего хранить между применениями

		UE_LOG(LogTemp, Warning, TEXT("LocalDamage received: %f"), LocalDamage);

		if (LocalDamage > 0.0f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - LocalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);

			const float Percent = GetMaxHealth() > 0.0f ? NewHealth / GetMaxHealth() : 0.0f;
			OnHealthChanged.Broadcast(Percent);

			if (NewHealth <= 0.0f && !bIsDead)
			{
				bIsDead = true;

				FGameplayTag DeathTag;
				const FGameplayTagContainer* SourceTags = Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
				if (SourceTags && SourceTags->Num() > 0)
				{
					DeathTag = SourceTags->First();
				}
				OnDeath.Broadcast(DeathTag);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// На случай, если что-то другое (лечение, дебаг) меняет Health напрямую —
		// просто пересчитываем процент, без повторной логики смерти
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
