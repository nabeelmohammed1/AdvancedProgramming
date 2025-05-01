// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

// forward-declares for types used only as pointers/references
class UCameraComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(config=Game)
class ADVANCEDPROGRAMMING_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

    /** 1st-person arms mesh (only visible to self) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta=(AllowPrivateAccess="true"))
    USkeletalMeshComponent* Mesh1P;

    /** First-person camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
    UCameraComponent* FirstPersonCameraComponent;

    /** Enhanced Input mapping context */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputMappingContext* DefaultMappingContext;

    /** Movement inputs */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess="true"))
    UInputAction* SprintAction;

    /** Sprint settings */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
    float SprintMultiplier = 2.0f;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
    float SprintDuration   = 3.0f;

public:
    APlayerCharacter();

protected:
    virtual void NotifyControllerChanged() override;
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void StartSprint();
    void StopSprint();

private:
    float DefaultMaxWalkSpeed;
    bool  bIsSprinting = false;
    FTimerHandle SprintTimerHandle;
};
