// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "HealthAttributeSet.h"
#include "AmmoAttributeSet.h"
#include "GASCharacterBase.generated.h"

UCLASS()
class STEALTHMISSION_API AGASCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGASCharacterBase();

	// Ability System Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UHealthAttributeSet* HealthAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	class UAmmoAttributeSet* AmmoAttributeSet;

	// GameplayEffect, задающий стартовые Health/MaxHealth — назначается в Blueprint-наследнике
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSubclassOf<class UGameplayEffect> DefaultHealthEffect;

	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem")
	TSubclassOf<class UGameplayEffect> DamageEffect; // Blueprint GE, Modifier: Health -= SetByCaller

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float Amount, FGameplayTag DamageTag);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilitySystem")
	EGameplayEffectReplicationMode AscReplicationMode = EGameplayEffectReplicationMode::Mixed;

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthChanged(float NewPercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnCharacterDeath(FGameplayTag DeathTag);

	void InitializeAttributes();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	bool bIsDead = false;
};
