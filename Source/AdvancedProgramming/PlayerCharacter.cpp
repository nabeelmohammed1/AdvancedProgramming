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
    // Capsule collision
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
    Mesh1P->CastShadow        = false;
    Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void APlayerCharacter::NotifyControllerChanged()
{
    Super::NotifyControllerChanged();
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Sub->AddMappingContext(DefaultMappingContext, 0);
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
    bCanSprint         = true;

    // Ensure mapping context
    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        if (auto Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
            Sub->AddMappingContext(DefaultMappingContext, 0);
    }
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
    Super::SetupPlayerInputComponent(InputComponent);

    if (auto EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Started,   this, &ACharacter::Jump);
        EnhancedInput->BindAction(JumpAction,   ETriggerEvent::Canceled,  this, &ACharacter::StopJumping);
        EnhancedInput->BindAction(MoveAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
        EnhancedInput->BindAction(LookAction,   ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Started,   this, &APlayerCharacter::StartSprint);
        EnhancedInput->BindAction(SprintAction, ETriggerEvent::Canceled,  this, &APlayerCharacter::StopSprint);
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    FVector2D Dir = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), Dir.Y);
    AddMovementInput(GetActorRightVector(),   Dir.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    FVector2D Delta = Value.Get<FVector2D>();
    AddControllerYawInput(Delta.X);
    AddControllerPitchInput(Delta.Y);
}

void APlayerCharacter::StartSprint()
{
    if (!bCanSprint || bIsSprinting)
        return;

    bIsSprinting = true;
    bCanSprint   = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * SprintMultiplier;
    GetWorld()->GetTimerManager().SetTimer(SprintTimerHandle, this, &APlayerCharacter::StopSprint, SprintDuration, false);
}

void APlayerCharacter::StopSprint()
{
    if (!bIsSprinting)
        return;

    bIsSprinting = false;
    GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
    GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &APlayerCharacter::ResetSprintCooldown, SprintCooldown, false);
}

void APlayerCharacter::ResetSprintCooldown()
{
    bCanSprint = true;
}
