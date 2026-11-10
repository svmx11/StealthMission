// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "HealthAttributeSet.h"
#include "AmmoAttributeSet.h"
#include "GASCharacterBase.generated.h"

class UGameplayAbility;
class UGameplayEffect;

// Смерть персонажа — для внешних подписчиков (зоны, тревога, менеджеры волн), без каста к конкретному классу
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDiedSignature, AGASCharacterBase*, DeadCharacter, FGameplayTag, DeathTag);

/**
 * Общая база для всех персонажей с GAS: игрок, противники.
 * Здесь только то, что одинаково для ВСЕХ: ASC, атрибуты, урон/смерть, команда.
 * Оружие — в компоненте BPC_WeaponManager (вешается в BP), ИИ-логика — в BP_EnemyBase.
 */
UCLASS()
class STEALTHMISSION_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGASCharacterBase();

	// ---------- Ability System ----------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UHealthAttributeSet* HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAmmoAttributeSet* AmmoAttributeSet;

	// GE стартового здоровья. Health и MaxHealth = SetByCaller Data.MaxHealth (значение берётся из DefaultMaxHealth)
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> DefaultHealthEffect;

	// GE урона: Damage (meta) += SetByCaller Data.Damage
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSubclassOf<UGameplayEffect> DamageEffect;

	// Стартовое и максимальное здоровье. У каждого типа противника своё
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem", meta = (ClampMin = "1.0"))
	float DefaultMaxHealth = 100.0f;

	// Неоружейные способности, выдаются один раз при инициализации ASC.
	// Оружейные (Fire/Aim/Reload) выдаёт BPC_WeaponManager из WeaponDataAsset
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	// ---------- Команда ----------

	// Team.Player / Team.Enemy / ... Пустой тег = ни с кем не союзник
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
	FGameplayTag TeamTag;

	// true, если Other — персонаж той же команды (сам себе тоже союзник)
	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsFriendlyTo(const AActor* Other) const;

	// true, если Other — персонаж другой команды. Ящики, стены и т.п. = false
	UFUNCTION(BlueprintPure, Category = "Team")
	bool IsHostileTo(const AActor* Other) const;

	// ---------- Бой ----------

	// Множитель разброса для GA_Fire. Игрок — 1. Противники переопределяют в BP (точность ИИ)
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Combat")
	float GetAimSpreadMultiplier() const;

	// ---------- Здоровье ----------

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	// Урон самому себе через DamageEffect. DamageTag — тип урона из иерархии Damage.* (Damage.Takedown, Damage.Fall…)
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount, FGameplayTag DamageTag);

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnCharacterDiedSignature OnDied;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthChanged(float NewPercent);

	// Получен урон (уже вычтен из Health). DamageInstigator — кто стрелял/ударил
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnDamageTaken(float Damage, AActor* DamageInstigator, FGameplayTag DamageType);

	// Вызывается один раз. До вызова способности уже отменены и повешен State.Dead
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnCharacterDeath(FGameplayTag DeathTag);

	// Идемпотентно: можно звать из BeginPlay и PossessedBy в любом порядке
	void InitAbilitySystem();
	void InitializeAttributes();
	void GiveStartupAbilities();
	virtual void HandleDeath(FGameplayTag DeathTag);

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	bool bIsDead = false;
	bool bAbilitySystemInitialized = false;
};