// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AISightTargetInterface.h"
#include "AISightCharacter.generated.h"

UCLASS()
class RTXGITEST_API AAISightCharacter : public ACharacter,public IAISightTargetInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(Category=AISight,EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess = "true"))
	TArray<FName> SightSockets;
	
	// Sets default values for this character's properties
	AAISightCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool CanBeSeenFrom(const FVector& ObserverLocation,FVector& OutSeenLocation,int32& NumberOfLoSChecksPerformed,float& OutSightStrength,const AActor* IgnoreActor = nullptr,const bool* bWasVisible = nullptr,int32* UserData = nullptr) const;
	//virtual bool CanBeSeenFrom(const FVector& ObserverLocation,FVector& OutSeenLocation,int32& NumberOfLoSChecksPerformed,float& OutSightStrength,const AActor* IgnoreActor = NULL) const;
};
