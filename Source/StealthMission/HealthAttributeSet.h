// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "HealthAttributeSet.generated.h"

// Однострочный макрос: без переноса строк обратной косой чертой, чтобы UHT не спотыкался на разборе директивы
// Своё имя, чтобы не конфликтовать с ATTRIBUTE_ACCESSORS из AmmoAttributeSet.h
#define HEALTH_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Здоровье: Health, MaxHealth и meta-атрибут Damage
 */
UCLASS()
class STEALTHMISSION_API UHealthAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UHealthAttributeSet();

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	HEALTH_ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Health)

		UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	HEALTH_ATTRIBUTE_ACCESSORS(UHealthAttributeSet, MaxHealth)

		// Meta-атрибут: GE пишет сюда урон, PostGameplayEffectExecute переводит его в Health и обнуляет
		UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData Damage;
	HEALTH_ATTRIBUTE_ACCESSORS(UHealthAttributeSet, Damage)

		DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, float /*NewHealthPercent*/);
	FOnHealthChanged OnHealthChanged;

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnDamageTaken, float /*Damage*/, AActor* /*Instigator*/, FGameplayTag /*DamageType*/);
	FOnDamageTaken OnDamageTaken;

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeath, FGameplayTag /*DeathTag*/);
	FOnDeath OnDeath;

	// Тип урона из спека: первый тег, подходящий под Damage.* (ищется в asset-тегах и source-тегах).
	// Теги стрелка (State.Armed, State.Aiming…) сюда больше не попадают
	static FGameplayTag ExtractDamageType(const FGameplayEffectSpec& Spec);

protected:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	bool bIsDead = false;
};