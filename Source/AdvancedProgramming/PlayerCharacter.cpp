// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Engine/LocalPlayer.h"

APlayerCharacter::APlayerCharacter()
{
    // Capsule setup
    GetCapsuleComponent()->InitCapsuleSize(55.f, 96.f);

    // Camera
    FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
    FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f));
    FirstPersonCameraComponent->bUsePawnControlRotation = true;

    // Arms mesh
    Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
    Mesh1P->SetOnlyOwnerSee(true);
    Mesh1P->SetupAttachment(FirstPersonCameraComponent);
    Mesh1P->bCastDynamicShadow = false;
    Mesh1P->CastShadow = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

    // Sprint defaults already in header
}

void APlayerCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (auto Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    if (auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Jump
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Started,   this, &ACharacter::Jump);
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Move & Look
        EnhancedInput->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        EnhancedInput->BindAction(LookAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

        // Sprint
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started,   this, &APlayerCharacter::StartSprint);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    const FVector2D Dir = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), Dir.Y);
    AddMovementInput(GetActorRightVector(),   Dir.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D Delta = Value.Get<FVector2D>();
    AddControllerYawInput(  Delta.X);
    AddControllerPitchInput(Delta.Y);
}

void APlayerCharacter::StartSprint()
{
    if (bIsSprinting) return;
    bIsSprinting = true;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * SprintMultiplier;
    GetWorldTimerManager().SetTimer(SprintTimerHandle, this, &APlayerCharacter::StopSprint, SprintDuration, false);
}

void APlayerCharacter::StopSprint()
{
    if (!bIsSprinting) return;
    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
    GetWorldTimerManager().ClearTimer(SprintTimerHandle);
}
