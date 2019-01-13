// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "MeleeBPLibrary.generated.h"

/**
 *
 */
UCLASS()
class TREDECIM_API UMeleeBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

		/*Does a collision trace against a character skeletal mesh and returns the first character hit*/
		UFUNCTION(BlueprintCallable, meta = (DisplayName = "Melee Trace", Keywords = "Trace melee mesh"), Category = Collision)
		static bool MeleeTrace(ACharacter* Instigator, float Distance, FVector StartLoc, FVector EndLoc, bool bDrawTrace, bool bTraceAgainstWorld, ECollisionChannel WorldTraceChannel, bool bWorldIgnoreCharacters, FHitResult& OutHit);

};
