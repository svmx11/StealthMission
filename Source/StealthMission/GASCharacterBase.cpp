// Fill out your copyright notice in the Description page of Project Settings.

#include "GASCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"

AGASCharacterBase::AGASCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(AscReplicationMode);

	HealthAttributeSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthAttributeSet"));
	AmmoAttributeSet = CreateDefaultSubobject<UAmmoAttributeSet>(TEXT("AmmoAttributeSet"));

	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
}

void AGASCharacterBase::BeginPlay()
{
	// ДО Super: Super::BeginPlay запускает BeginPlay компонентов (BPC_WeaponManager),
	// и к этому моменту ASC уже должен быть готов — у игрока Possess приходит позже BeginPlay
	InitAbilitySystem();
	Super::BeginPlay();
}

void AGASCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// Обновит ActorInfo (теперь с контроллером). Повторно атрибуты не сбросит и не подпишется второй раз
	InitAbilitySystem();
}

void AGASCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void AGASCharacterBase::InitAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (bAbilitySystemInitialized)
	{
		return;
	}
	bAbilitySystemInitialized = true;

	InitializeAttributes();
	GiveStartupAbilities();

	HealthAttributeSet->OnHealthChanged.AddUObject(this, &AGASCharacterBase::OnHealthChanged);
	HealthAttributeSet->OnDamageTaken.AddUObject(this, &AGASCharacterBase::OnDamageTaken);
	HealthAttributeSet->OnDeath.AddUObject(this, &AGASCharacterBase::HandleDeath);
}

void AGASCharacterBase::InitializeAttributes()
{
	if (!HasAuthority() || !AbilitySystemComponent || !DefaultHealthEffect)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultHealthEffect, 1.0f, EffectContext);
	if (SpecHandle.IsValid())
	{
		static const FGameplayTag MaxHealthTag = FGameplayTag::RequestGameplayTag(TEXT("Data.MaxHealth"));
		SpecHandle.Data->SetSetByCallerMagnitude(MaxHealthTag, DefaultMaxHealth);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AGASCharacterBase::GiveStartupAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent)
	{
		return;
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		if (AbilityClass)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
		}
	}
}

void AGASCharacterBase::HandleDeath(FGameplayTag DeathTag)
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();

		// Способности с Activation Blocked Tags = State.Dead больше не запустятся
		static const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	OnCharacterDeath(DeathTag);
	OnDied.Broadcast(this, DeathTag);
}

float AGASCharacterBase::GetHealthPercent() const
{
	if (!HealthAttributeSet || HealthAttributeSet->GetMaxHealth() <= 0.0f)
	{
		return 0.0f;
	}
	return HealthAttributeSet->GetHealth() / HealthAttributeSet->GetMaxHealth();
}

void AGASCharacterBase::ApplyDamage(float Amount, FGameplayTag DamageTag)
{
	if (!AbilitySystemComponent || !DamageEffect)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageEffect, 1.0f, EffectContext);
	if (SpecHandle.IsValid())
	{
		static const FGameplayTag DamageDataTag = FGameplayTag::RequestGameplayTag(TEXT("Data.Damage"));
		SpecHandle.Data->SetSetByCallerMagnitude(DamageDataTag, Amount);
		if (DamageTag.IsValid())
		{
			SpecHandle.Data->CapturedSourceTags.GetSpecTags().AddTag(DamageTag);
		}
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

bool AGASCharacterBase::IsFriendlyTo(const AActor* Other) const
{
	const AGASCharacterBase* OtherCharacter = Cast<AGASCharacterBase>(Other);
	return OtherCharacter && TeamTag.IsValid() && OtherCharacter->TeamTag == TeamTag;
}

bool AGASCharacterBase::IsHostileTo(const AActor* Other) const
{
	const AGASCharacterBase* OtherCharacter = Cast<AGASCharacterBase>(Other);
	return OtherCharacter && TeamTag.IsValid() && OtherCharacter->TeamTag.IsValid()
		&& OtherCharacter->TeamTag != TeamTag;
}

float AGASCharacterBase::GetAimSpreadMultiplier_Implementation() const
{
	return 1.0f;
}

bool AGASCharacterBase::IsCrouchingForAnim_Implementation() const
{
	return bIsCrouched;
}

bool AGASCharacterBase::IsWeaponRaisedForAnim_Implementation() const
{
	static const FGameplayTag WeaponRaisedTag = FGameplayTag::RequestGameplayTag(TEXT("State.WeaponRaised"));
	return AbilitySystemComponent && AbilitySystemComponent->HasMatchingGameplayTag(WeaponRaisedTag);
}

UAbilitySystemComponent* AGASCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}