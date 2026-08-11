// Fill out your copyright notice in the Description page of Project Settings.



#include "WeaponManagerComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UWeaponManagerComponent::UWeaponManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

    if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
    {
        EquippedMeshComponent = NewObject<UStaticMeshComponent>(OwnerChar, TEXT("EquippedWeaponMesh"));
        EquippedMeshComponent->SetupAttachment(OwnerChar->GetMesh());
        EquippedMeshComponent->RegisterComponent();
        EquippedMeshComponent->SetVisibility(false);
    }

    if (DefaultWeaponData)
    {
        EquipWeapon(DefaultWeaponData);
    }
}

void UWeaponManagerComponent::EquipWeapon(UWeaponDataAsset* NewWeaponData)
{
    if (!NewWeaponData || NewWeaponData == CurrentWeaponData)
    {
        return;
    }

    UWeaponDataAsset* OldWeapon = CurrentWeaponData;
    CurrentWeaponData = NewWeaponData;

    AttachWeaponMesh(NewWeaponData);
    UpdateWeaponTag(OldWeapon, NewWeaponData);

    OnWeaponChanged.Broadcast(CurrentWeaponData);
}

bool UWeaponManagerComponent::IsArmed() const
{
    return CurrentWeaponData && CurrentWeaponData->WeaponTag != FGameplayTag::RequestGameplayTag(FName("Weapon.Unarmed"));
}

void UWeaponManagerComponent::AttachWeaponMesh(UWeaponDataAsset* WeaponData)
{
    if (!EquippedMeshComponent)
    {
        return;
    }

    if (!WeaponData->WeaponMesh)
    {
        EquippedMeshComponent->SetVisibility(false);
        return;
    }

    EquippedMeshComponent->SetStaticMesh(WeaponData->WeaponMesh);

    if (ACharacter* OwnerChar = Cast<ACharacter>(GetOwner()))
    {
        EquippedMeshComponent->AttachToComponent(
            OwnerChar->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            WeaponData->AttachSocketName);
    }

    EquippedMeshComponent->SetVisibility(true);
}

void UWeaponManagerComponent::UpdateWeaponTag(UWeaponDataAsset* OldWeapon, UWeaponDataAsset* NewWeapon)
{
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
    if (!ASI)
    {
        return;
    }

    UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    if (OldWeapon)
    {
        ASC->RemoveLooseGameplayTag(OldWeapon->WeaponTag);
    }
    if (NewWeapon)
    {
        ASC->AddLooseGameplayTag(NewWeapon->WeaponTag);
    }
}
