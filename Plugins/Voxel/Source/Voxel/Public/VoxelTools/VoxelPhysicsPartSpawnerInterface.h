// Copyright 2020 Phyronnaz

#pragma once

#include "CoreMinimal.h"
#include "VoxelMaterial.h"
#include "VoxelMinimal.h"
#include "UObject/Interface.h"
#include "VoxelPhysicsPartSpawnerInterface.generated.h"

class AStaticMeshActor;
class UStaticMesh;
class UMaterialInterface;
class FVoxelData;
class AVoxelWorld;

USTRUCT(BlueprintType)
struct FVoxelPositionValueMaterial
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	FIntVector Position = FIntVector(ForceInit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	float Value = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Voxel")
	FVoxelMaterial Material = FVoxelMaterial(ForceInit);
};

// Represents the result for a single part
UINTERFACE(BlueprintType)
class VOXEL_API UVoxelPhysicsPartSpawnerResult : public UInterface
{
	GENERATED_BODY()
};

class VOXEL_API IVoxelPhysicsPartSpawnerResult : public IInterface
{
	GENERATED_BODY()
};


UINTERFACE(BlueprintType)
class VOXEL_API UVoxelPhysicsPartSpawner : public UInterface
{
	GENERATED_BODY()
};

class VOXEL_API IVoxelPhysicsPartSpawner : public IInterface
{
	GENERATED_BODY()
	
public:
	/**
	 * @param OutOnWorldUpdateDone	Will be triggered once the world update of the removed voxels will be done
	 * @param World		            The voxel world
	 * @param Data		            The data of this voxel part
	 * @param Voxels				Voxels in this part
	 * @param PartPosition	        The position of this part in the voxel world voxel space
	 */
	virtual TScriptInterface<IVoxelPhysicsPartSpawnerResult> SpawnPart(
		TVoxelSharedPtr<FSimpleDelegate>& OutOnWorldUpdateDone,
		AVoxelWorld* World,
		TVoxelSharedPtr<FVoxelData>&& Data,
		TArray<FVoxelPositionValueMaterial>&& Voxels,
		const FIntVector& PartPosition)
	{
		return nullptr;
	}

	virtual bool NeedData() const { return false; }
	virtual bool NeedVoxels() const { return false; }
};
