// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeBPLibrary.h"


UMeleeBPLibrary::UMeleeBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}


bool UMeleeBPLibrary::MeleeTrace(ACharacter* Instigator, float Distance, FVector StartLoc, FVector EndLoc, bool bDrawTrace, bool bTraceAgainstWorld, ECollisionChannel WorldTraceChannel, bool bWorldIgnoreCharacters, FHitResult& OutHit)
{
	FHitResult HitData(ForceInit);
	FCollisionQueryParams Params;
	Params.bReturnPhysicalMaterial = true;

	/* we need a instigator before tracing*/
	if (!Instigator) { return false; }

	if (bDrawTrace)
	{
		DrawDebugLine(Instigator->GetWorld(), StartLoc, EndLoc, FColor::Red, false, 3, 0, 1.0);
	}


	/*If we should trace against the world*/
	if (bTraceAgainstWorld)
	{
		/* then do a trace against the world*/
		if (Instigator->GetWorld()->LineTraceSingleByChannel(HitData, StartLoc, EndLoc, WorldTraceChannel, Params))
		{
			/* ignore characters in the world trace*/
			if (!Cast<ACharacter>(HitData.GetActor()))
			{
				/*if it hits, return true*/
				OutHit = HitData;
				return true;
			}
			else
			{
				if (!bWorldIgnoreCharacters)
				{
					OutHit = HitData;
					return true;
				}
			}

		}
	}


	/* loop trough all of the pawns in the world*/
	for (FConstPawnIterator Iterator = Instigator->GetWorld()->GetPawnIterator(); Iterator; ++Iterator) // Lookup for all the pawns in the world
	{

		ACharacter* Char = Cast<ACharacter>(*Iterator);


		// ignore the instigator
		if (Char && Char != Instigator)
		{

			// optimization - grab only pawns close to the instigator
			if (Instigator->GetDistanceTo(Char) <= Distance)
			{

				// if the trace hits against the pawn mesh
				if (Char->GetMesh()->LineTraceComponent(HitData, StartLoc, EndLoc, Params))
				{
					// draw the debug line and return true
					DrawDebugLine(Instigator->GetWorld(), StartLoc, EndLoc, FColor::Green, false, 1, 0, 0.5);
					OutHit = HitData;
					return true;
					break;
				}


			}
			else
			{
				return false;
			}
		}
	}
	return false;

}