// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAISense_Sight.h"
#include "EngineDefines.h"
#include "EngineGlobals.h"
#include "CollisionQueryParams.h"
#include "Engine/Engine.h"
#include "AISystem.h"
#include "AIHelpers.h"
#include "Perception/AIPerceptionComponent.h"
#include "VisualLogger/VisualLogger.h"
#include "Perception/AISightTargetInterface.h"
#include "Perception/AISenseConfig_Sight.h"


//void UMyAISense_Sight::PostInitProperties()
//{
//	Super::PostInitProperties();
//}


//#define DO_SIGHT_VLOGGING (0 && ENABLE_VISUAL_LOG)
//
//#if DO_SIGHT_VLOGGING
//#define SIGHT_LOG_SEGMENT(LogOwner, SegmentStart, SegmentEnd, Color, Format, ...) UE_VLOG_SEGMENT(LogOwner, LogAIPerception, Verbose, SegmentStart, SegmentEnd, Color, Format, ##__VA_ARGS__)
//#define SIGHT_LOG_LOCATION(LogOwner, Location, Radius, Color, Format, ...) UE_VLOG_LOCATION(LogOwner, LogAIPerception, Verbose, Location, Radius, Color, Format, ##__VA_ARGS__)
//#else
//#define SIGHT_LOG_SEGMENT(...)
//#define SIGHT_LOG_LOCATION(...)
//#endif // DO_SIGHT_VLOGGING
//
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight"),STAT_AI_Sense_Sight,STATGROUP_AI);
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight, Update Sort"),STAT_AI_Sense_Sight_UpdateSort,STATGROUP_AI);
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight, Listener Update"),STAT_AI_Sense_Sight_ListenerUpdate,STATGROUP_AI);
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight, Register Target"),STAT_AI_Sense_Sight_RegisterTarget,STATGROUP_AI);
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight, Remove By Listener"),STAT_AI_Sense_Sight_RemoveByListener,STATGROUP_AI);
//DECLARE_CYCLE_STAT(TEXT("Perception Sense: Sight, Remove To Target"),STAT_AI_Sense_Sight_RemoveToTarget,STATGROUP_AI);
//
//enum class EForEachResult: uint8
//{
//	Break,
//	Continue,
//};
//
//template <typename T,class PREDICATE_CLASS>
//EForEachResult ForEach(T& Array,const PREDICATE_CLASS& Predicate)
//{
//	for(typename T::ElementType& Element : Array)
//	{
//		if(Predicate(Element) == EForEachResult::Break)
//		{
//			return EForEachResult::Break;
//		}
//	}
//	return EForEachResult::Continue;
//}
//
//enum EReverseForEachResult: uint8
//{
//	UnTouched,
//	Modified,
//};

//float UMyAISense_Sight::Update()
//{
//	SCOPE_CYCLE_COUNTER(STAT_AI_Sense_Sight);
//
//	const UWorld* World = GEngine->GetWorldFromContextObject(GetPerceptionSystem()->GetOuter(),EGetWorldErrorMode::LogAndReturnNull);
//
//	if(World == NULL)
//	{
//		return SuspendNextUpdate;
//	}
//
//	// sort Sight Queries
//	{
//		auto RecalcScore = [](FAISightQuery& SightQuery)->EForEachResult
//		{
//			SightQuery.RecalcScore();
//			return EForEachResult::Continue;
//		};
//
//		SCOPE_CYCLE_COUNTER(STAT_AI_Sense_Sight_UpdateSort);
//		// Sort out of range queries
//		if(bSightQueriesOutOfRangeDirty)
//		{
//			ForEach(SightQueriesOutOfRange,RecalcScore);
//			SightQueriesOutOfRange.Sort(FAISightQuery::FSortPredicate());
//			NextOutOfRangeIndex = 0;
//			bSightQueriesOutOfRangeDirty = false;
//		}
//
//		// Sort in range queries
//		ForEach(SightQueriesInRange,RecalcScore);
//		SightQueriesInRange.Sort(FAISightQuery::FSortPredicate());
//	}
//
//	int32 TracesCount = 0;
//	int32 NumQueriesProcessed = 0;
//	double TimeSliceEnd = FPlatformTime::Seconds() + MaxTimeSlicePerTick;
//	bool bHitTimeSliceLimit = false;
//	//#define AISENSE_SIGHT_TIMESLICING_DEBUG
//#ifdef AISENSE_SIGHT_TIMESLICING_DEBUG
//	double TimeSpent = 0.0;
//	double LastTime = FPlatformTime::Seconds();
//#endif // AISENSE_SIGHT_TIMESLICING_DEBUG
//	static const int32 InitialInvalidItemsSize = 16;
//	enum class EOperationType: uint8
//	{
//		Remove,
//		SwapList
//	};
//	struct FQueryOperation
//	{
//		FQueryOperation(bool bInInRange,EOperationType InOpType,int32 InIndex): bInRange(bInInRange),OpType(InOpType),Index(InIndex) {}
//		bool bInRange;
//		EOperationType OpType;
//		int32 Index;
//	};
//	TArray<FQueryOperation> QueryOperations;
//	TArray<FAISightTarget::FTargetId> InvalidTargets;
//	QueryOperations.Reserve(InitialInvalidItemsSize);
//	InvalidTargets.Reserve(InitialInvalidItemsSize);
//
//	AIPerception::FListenerMap& ListenersMap = *GetListeners();
//
//	int32 InRangeItr = 0;
//	int32 OutOfRangeItr = 0;
//	for(int32 QueryIndex = 0; QueryIndex < SightQueriesInRange.Num() + SightQueriesOutOfRange.Num(); ++QueryIndex)
//	{
//		// Calculate next in range query
//		int32 InRangeIndex = SightQueriesInRange.IsValidIndex(InRangeItr) ? InRangeItr : INDEX_NONE;
//		FAISightQuery* InRangeQuery = InRangeIndex != INDEX_NONE ? &SightQueriesInRange[InRangeIndex] : nullptr;
//
//		// Calculate next out of range query
//		int32 OutOfRangeIndex = SightQueriesOutOfRange.IsValidIndex(OutOfRangeItr) ? (NextOutOfRangeIndex + OutOfRangeItr) % SightQueriesOutOfRange.Num() : INDEX_NONE;
//		FAISightQuery* OutOfRangeQuery = OutOfRangeIndex != INDEX_NONE ? &SightQueriesOutOfRange[OutOfRangeIndex] : nullptr;
//		if(OutOfRangeQuery)
//		{
//			OutOfRangeQuery->RecalcScore();
//		}
//
//		// Compare to real find next query
//		const bool bIsInRangeQuery = (InRangeQuery && OutOfRangeQuery) ? FAISightQuery::FSortPredicate()(*InRangeQuery,*OutOfRangeQuery) : !OutOfRangeQuery;
//		FAISightQuery* SightQuery = bIsInRangeQuery ? InRangeQuery : OutOfRangeQuery;
//
//		// Time slice limit check - spread out checks to every N queries so we don't spend more time checking timer than doing work
//		NumQueriesProcessed++;
//#ifdef AISENSE_SIGHT_TIMESLICING_DEBUG
//		TimeSpent += (FPlatformTime::Seconds() - LastTime);
//		LastTime = FPlatformTime::Seconds();
//#endif // AISENSE_SIGHT_TIMESLICING_DEBUG
//		if(bHitTimeSliceLimit == false && (NumQueriesProcessed % MinQueriesPerTimeSliceCheck) == 0 && FPlatformTime::Seconds() > TimeSliceEnd)
//		{
//			bHitTimeSliceLimit = true;
//			// do not break here since that would bypass queue aging
//		}
//
//		if(TracesCount < MaxTracesPerTick && bHitTimeSliceLimit == false)
//		{
//			bIsInRangeQuery ? ++InRangeItr : ++OutOfRangeItr;
//
//			FPerceptionListener& Listener = ListenersMap[SightQuery->ObserverId];
//			FAISightTarget& Target = ObservedTargets[SightQuery->TargetId];
//
//			AActor* TargetActor = Target.Target.Get();
//			UAIPerceptionComponent* ListenerPtr = Listener.Listener.Get();
//			ensure(ListenerPtr);
//
//			// @todo figure out what should we do if not valid
//			if(TargetActor && ListenerPtr)
//			{
//				const FVector TargetLocation = TargetActor->GetActorLocation();
//				const FDigestedSightProperties& PropDigest = DigestedProperties[SightQuery->ObserverId];
//				const float SightRadiusSq = SightQuery->bLastResult ? PropDigest.LoseSightRadiusSq : PropDigest.SightRadiusSq;
//
//				float StimulusStrength = 1.f;
//
//				// @Note that automagical "seeing" does not care about sight range nor vision cone
//				const bool bShouldAutomatically = ShouldAutomaticallySeeTarget(PropDigest,SightQuery,Listener,TargetActor,StimulusStrength);
//				if(bShouldAutomatically)
//				{
//					// Pretend like we've seen this target where we last saw them
//					Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,StimulusStrength,SightQuery->LastSeenLocation,Listener.CachedLocation));
//					SightQuery->bLastResult = true;
//				}
//				else if(FAISystem::CheckIsTargetInSightCone(Listener.CachedLocation,Listener.CachedDirection,PropDigest.PeripheralVisionAngleCos,PropDigest.PointOfViewBackwardOffset,PropDigest.NearClippingRadiusSq,SightRadiusSq,TargetLocation))
//				{
//					SIGHT_LOG_SEGMENT(ListenerPtr->GetOwner(),Listener.CachedLocation,TargetLocation,FColor::Green,TEXT("TargetID %d"),Target.TargetId);
//
//					FVector OutSeenLocation(0.f);
//					// do line checks
//					if(Target.SightTargetInterface != NULL)
//					{
//						int32 NumberOfLoSChecksPerformed = 0;
//						// defaulting to 1 to have "full strength" by default instead of "no strength"
//						const bool bWasVisible = SightQuery->bLastResult;
//						if(Target.SightTargetInterface->CanBeSeenFrom(Listener.CachedLocation,OutSeenLocation,NumberOfLoSChecksPerformed,StimulusStrength,ListenerPtr->GetBodyActor(),&bWasVisible,&SightQuery->UserData) == true)
//						{
//							Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,StimulusStrength,OutSeenLocation,Listener.CachedLocation));
//							SightQuery->bLastResult = true;
//							SightQuery->LastSeenLocation = OutSeenLocation;
//						}
//						// communicate failure only if we've seen give actor before
//						else if(SightQuery->bLastResult == true)
//						{
//							Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,0.f,TargetLocation,Listener.CachedLocation,FAIStimulus::SensingFailed));
//							SightQuery->bLastResult = false;
//							SightQuery->LastSeenLocation = FAISystem::InvalidLocation;
//						}
//
//						if(SightQuery->bLastResult == false)
//						{
//							SIGHT_LOG_LOCATION(ListenerPtr->GetOwner(),TargetLocation,25.f,FColor::Red,TEXT(""));
//						}
//
//						TracesCount += NumberOfLoSChecksPerformed;
//					}
//					else
//					{
//						// we need to do tests ourselves
//						FHitResult HitResult;
//						const bool bHit = World->LineTraceSingleByChannel(HitResult,Listener.CachedLocation,TargetLocation
//							,DefaultSightCollisionChannel
//							,FCollisionQueryParams(SCENE_QUERY_STAT(AILineOfSight),true,ListenerPtr->GetBodyActor()));
//
//						++TracesCount;
//
//						auto HitResultActorIsOwnedByTargetActor = [&HitResult,TargetActor]()
//						{
//							AActor* HitResultActor = HitResult.Actor.Get();
//							return (HitResultActor ? HitResultActor->IsOwnedBy(TargetActor) : false);
//						};
//
//						if(bHit == false || HitResultActorIsOwnedByTargetActor())
//						{
//							Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,1.f,TargetLocation,Listener.CachedLocation));
//							SightQuery->bLastResult = true;
//							SightQuery->LastSeenLocation = TargetLocation;
//						}
//						// communicate failure only if we've seen give actor before
//						else if(SightQuery->bLastResult == true)
//						{
//							Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,0.f,TargetLocation,Listener.CachedLocation,FAIStimulus::SensingFailed));
//							SightQuery->bLastResult = false;
//							SightQuery->LastSeenLocation = FAISystem::InvalidLocation;
//						}
//
//						if(SightQuery->bLastResult == false)
//						{
//							SIGHT_LOG_LOCATION(ListenerPtr->GetOwner(),TargetLocation,25.f,FColor::Red,TEXT(""));
//						}
//					}
//				}
//				// communicate failure only if we've seen give actor before
//				else if(SightQuery->bLastResult)
//				{
//					SIGHT_LOG_SEGMENT(ListenerPtr->GetOwner(),Listener.CachedLocation,TargetLocation,FColor::Red,TEXT("TargetID %d"),Target.TargetId);
//					Listener.RegisterStimulus(TargetActor,FAIStimulus(*this,0.f,TargetLocation,Listener.CachedLocation,FAIStimulus::SensingFailed));
//					SightQuery->bLastResult = false;
//				}
//
//				SightQuery->Importance = CalcQueryImportance(Listener,TargetLocation,SightRadiusSq);
//				const bool bShouldBeInRange = SightQuery->Importance > 0.0f;
//				if(bIsInRangeQuery != bShouldBeInRange)
//				{
//					QueryOperations.Add(FQueryOperation(bIsInRangeQuery,EOperationType::SwapList,bIsInRangeQuery ? InRangeIndex : OutOfRangeIndex));
//				}
//
//				// restart query
//				SightQuery->OnProcessed();
//			}
//			else
//			{
//				// put this index to "to be removed" array
//				QueryOperations.Add(FQueryOperation(bIsInRangeQuery,EOperationType::Remove,bIsInRangeQuery ? InRangeIndex : OutOfRangeIndex));
//				if(TargetActor == nullptr)
//				{
//					InvalidTargets.AddUnique(SightQuery->TargetId);
//				}
//			}
//		}
//		else
//		{
//			break;
//		}
//	}
//	NextOutOfRangeIndex = SightQueriesOutOfRange.Num() > 0 ? (NextOutOfRangeIndex + OutOfRangeItr) % SightQueriesOutOfRange.Num() : 0;
//
//#ifdef AISENSE_SIGHT_TIMESLICING_DEBUG
//	UE_LOG(LogAIPerception,VeryVerbose,TEXT("UAISense_Sight::Update processed %d sources in %f seconds [time slice limited? %d]"),NumQueriesProcessed,TimeSpent,bHitTimeSliceLimit ? 1 : 0);
//#else
//	UE_LOG(LogAIPerception,VeryVerbose,TEXT("UAISense_Sight::Update processed %d sources [time slice limited? %d]"),NumQueriesProcessed,bHitTimeSliceLimit ? 1 : 0);
//#endif // AISENSE_SIGHT_TIMESLICING_DEBUG
//
//	if(QueryOperations.Num() > 0)
//	{
//		// Sort by InRange and by descending Index 
//		QueryOperations.Sort([](const FQueryOperation& LHS,const FQueryOperation& RHS)->bool
//		{
//			if(LHS.bInRange != RHS.bInRange)
//				return LHS.bInRange;
//			return LHS.Index > RHS.Index;
//		});
//		// Do all the removes first and save the out of range swaps because we will insert them at the right location to prevent sorting
//		TArray<FAISightQuery> SightQueriesOutOfRangeToInsert;
//		for(FQueryOperation& Operation : QueryOperations)
//		{
//			if(Operation.OpType == EOperationType::SwapList)
//			{
//				if(Operation.bInRange)
//				{
//					SightQueriesOutOfRangeToInsert.Push(SightQueriesInRange[Operation.Index]);
//				}
//				else
//				{
//					SightQueriesInRange.Add(SightQueriesOutOfRange[Operation.Index]);
//				}
//			}
//
//			if(Operation.bInRange)
//			{
//				// In range queries are always sorted at the beginning of the update
//				SightQueriesInRange.RemoveAtSwap(Operation.Index,1, /*bAllowShrinking*/false);
//			}
//			else
//			{
//				// Preserve the list ordered
//				SightQueriesOutOfRange.RemoveAt(Operation.Index,1, /*bAllowShrinking*/false);
//				if(Operation.Index < NextOutOfRangeIndex)
//				{
//					NextOutOfRangeIndex--;
//				}
//			}
//		}
//		// Reinsert the saved out of range swaps
//		if(SightQueriesOutOfRangeToInsert.Num() > 0)
//		{
//			SightQueriesOutOfRange.Insert(SightQueriesOutOfRangeToInsert.GetData(),SightQueriesOutOfRangeToInsert.Num(),NextOutOfRangeIndex);
//			NextOutOfRangeIndex += SightQueriesOutOfRangeToInsert.Num();
//		}
//
//		if(InvalidTargets.Num() > 0)
//		{
//			// this should not be happening since UAIPerceptionSystem::OnPerceptionStimuliSourceEndPlay introduction
//			UE_VLOG(GetPerceptionSystem(),LogAIPerception,Error,TEXT("Invalid sight targets found during UAISense_Sight::Update call"));
//
//			for(const auto& TargetId : InvalidTargets)
//			{
//				// remove affected queries
//				RemoveAllQueriesToTarget(TargetId);
//				// remove target itself
//				ObservedTargets.Remove(TargetId);
//			}
//
//			// remove holes
//			ObservedTargets.Compact();
//		}
//	}
//
//	//return SightQueries.Num() > 0 ? 1.f/6 : FLT_MAX;
//	return 0.f;
//}
