// Fill out your copyright notice in the Description page of Project Settings.


#include "AISightCharacter.h"

// Sets default values
AAISightCharacter::AAISightCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAISightCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAISightCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAISightCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//bool AAISightCharacter::CanBeSeenFrom(const FVector& ObserverLocation,FVector& OutSeenLocation,int32& NumberOfLoSChecksPerformed,float& OutSightStrength,const AActor* IgnoreActor) const
bool AAISightCharacter::CanBeSeenFrom(const FVector& ObserverLocation,FVector& OutSeenLocation,int32& NumberOfLoSChecksPerformed,float& OutSightStrength,const AActor* IgnoreActor,const bool* bWasVisible,int32* UserData) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));

	FHitResult HitResult;

	//auto sockets = GetMesh()->GetAllSocketNames();

	for(int i = 0; i < SightSockets.Num(); i++)
	{
		FVector socketLocation = GetMesh()->GetSocketLocation(SightSockets[i]);

		const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult,ObserverLocation,socketLocation
			,FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)) // << Changed this line
			,FCollisionQueryParams(NAME_AILineOfSight,true,IgnoreActor));

		NumberOfLoSChecksPerformed++;

		if(bHitSocket == false || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this))) {
			OutSeenLocation = socketLocation;
			OutSightStrength = 1;

			return true;
		}
	}

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult,ObserverLocation,GetActorLocation()
		,FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)) // << Changed this line
		,FCollisionQueryParams(NAME_AILineOfSight,true,IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if(bHit == false || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;

		return true;
	}

	OutSightStrength = 0;
	return false;
}