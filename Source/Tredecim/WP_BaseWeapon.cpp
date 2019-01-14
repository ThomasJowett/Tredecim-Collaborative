// Fill out your copyright notice in the Description page of Project Settings.

#include "WP_BaseWeapon.h"


// Sets default values
AWP_BaseWeapon::AWP_BaseWeapon(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	/* initialize the mesh component*/
	Mesh = ObjectInitializer.CreateDefaultSubobject < USkeletalMeshComponent >(this, TEXT("WeaponMesh"));
	RootComponent = Mesh;
	PrimaryActorTick.bCanEverTick = true;
}

bool AWP_BaseWeapon::CheckHurtEnemies(ACharacter* HurtChar)
{
	int i;

	/* find the character on the array*/
	i = EnemiesHitOnThisSwing.Find(HurtChar);

	/* make sure we are accesing a valid idnex*/
	if (EnemiesHitOnThisSwing.IsValidIndex(i)) // 
	{

		/* if the character exists on the array, then we have alredy hit him*/
		if (EnemiesHitOnThisSwing[i]->IsValidLowLevel()) // 
		{
			return false;
		}
		else
			/* if he doesnt, then add him to the list*/
			EnemiesHitOnThisSwing.Add(HurtChar);
		return true;

	}
	EnemiesHitOnThisSwing.Add(HurtChar);
	return true;
}


bool AWP_BaseWeapon::CheckHurtActors(AActor* HurtActor)
{
	int i;

	/* find the character on the array*/
	i = ActorsHitOnThisSwing.Find(HurtActor);

	/* make sure we are accesing a valid idnex*/
	if (ActorsHitOnThisSwing.IsValidIndex(i)) // 
	{

		/* if the character exists on the array, then we have alredy hit him*/
		if (ActorsHitOnThisSwing[i]->IsValidLowLevel()) // 
		{
			return false;
		}
		else
			/* if he doesnt, then add him to the list*/
			ActorsHitOnThisSwing.Add(HurtActor);
		return true;

	}
	ActorsHitOnThisSwing.Add(HurtActor);
	return true;
}

void AWP_BaseWeapon::BeginSwing()
{

	/*Dont trace if we dont have a valid weapon owner*/
	if (!WeaponOwner->IsValidLowLevel())
	{
		return;
	}

	/* tell the weapon to start tracing*/
	bTrace = true;

	/* initialize the vectors for the tracing*/
	LastFrameStartSockEnd = Mesh->GetSocketLocation(SocketStartName);
	LastFrameEndSockEnd = EndSockEnd = Mesh->GetSocketLocation(SocketEndName);
}

void AWP_BaseWeapon::EndSwing()
{
	bTrace = false;
	/* empty the hit list*/
	EnemiesHitOnThisSwing.Empty();

	/* if we are performing a trace against the world*/
	if (bTraceAgainstWorld)
	{
		//then empty the actors hit list
		ActorsHitOnThisSwing.Empty();
	}
}

void AWP_BaseWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FVector TraceStart;
	FVector TraceEnd;
	FVector Norm;
	FVector Hit;
	FName F;
	int i;
	FHitResult HitData(ForceInit);
	FCollisionQueryParams Params;

	//Parameters for the objects line trace
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	Params.bTraceComplex = true;


	/* if we should trace*/
	if (bTrace)
	{
		/*Start by getting the start and end location from the sockets for the traces*/
		StartSockEnd = Mesh->GetSocketLocation(SocketStartName); // get the start and end location to make our traes from
		EndSockEnd = Mesh->GetSocketLocation(SocketEndName);

		/*For the amount of traces this weapon should do*/
		for (i = 0; i < TracesPerSwing; i++)
		{
			/*Make up the traces for the amount needed with the 2 socket locations*/
			TraceStart = FMath::Lerp(StartSockEnd, EndSockEnd, i / TracesPerSwing);
			/*And lerp between the ends of the previous frame to NOT miss any traces */
			TraceEnd = FMath::Lerp(LastFrameStartSockEnd, LastFrameEndSockEnd, i / TracesPerSwing);

			/*If the flag is true, also trace against the world*/
			if (bTraceAgainstWorld)
			{
				MeleeTraceWorld(TraceStart, TraceEnd);
			}


			if (bDrawTraces)
			{
				DrawDebugLine(Instigator->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5, 0, 2.0);
			}

			/* Loop trough all of the pawns in the world*/
			for (FConstPawnIterator Iterator = GetWorld()->GetPawnIterator(); Iterator; ++Iterator)
			{
				/* store one of them in a variable for easy access*/
				ACharacter* Pawn = Cast<ACharacter>(*Iterator);
				/* If we get a pawn and its not us*/
				if (Pawn && Pawn != WeaponOwner) //
				{
					/* Check if he is within the acceptable melee distance*/
					if (WeaponOwner->GetDistanceTo(Pawn) <= MeleeRadius)
					{

						//GetWorld()->LineTrace
						//If the pawn gets hit by a normal trace -- On 4.14 Using LineTraceComponent gives the wrong hit results so we use a standard trace instead
						if (Pawn->GetMesh()->LineTraceComponent(HitData, TraceStart, TraceEnd, Params))
						{ // -> trace only for pawns

							/* if we havent hit the pawn on this swing, then do so*/
							if (HitData.GetActor() != WeaponOwner && HitData.GetActor() == Pawn && CheckHurtEnemies(Pawn) == true)
							{
								/*You can implement your damage dealing here or whatever else you want to happen
								When the weapon has hit the character*/
								if (HitData.GetActor() != this->WeaponOwner)
								{
									HitData.GetActor()->TakeDamage(bLightAttack ? LightDamage : HeavyDamage, FDamageEvent(), WeaponOwner->GetInstigatorController(), this);
									OnHit(HitData);
									HitBone = HitData.BoneName;
								}
							}
						}
					}
				}
			}
		}

		/* Store the location of the previous frame*/
		LastFrameStartSockEnd = StartSockEnd;
		LastFrameEndSockEnd = EndSockEnd;
	}

}

void AWP_BaseWeapon::MeleeTraceWorld(FVector TraceStart, FVector TraceEnd)
{

	FHitResult HitData(ForceInit);
	FCollisionQueryParams Params;
	//Draw the debug traces if the flag is true
	if (bDrawTraces)
	{
		DrawDebugLine(Instigator->GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 5, 0, 2.0);
	}
	/*if the trace hits something*/
	if (GetWorld()->LineTraceSingleByChannel(HitData, TraceStart, TraceEnd, ECC_Visibility, Params))
	{
		/* and we havent hit that " something " before*/
		if (CheckHurtActors(HitData.GetActor()) == true)
		{
			/*You can implement your damage dealing here or whatever else you want to happen
			When the weapon has hit the actor here*/
			if (HitData.GetActor() != this->WeaponOwner)
			{
				/* make sure we did not hit a character, since thats taken care of in the other trace*/
				if (!Cast<ACharacter>(HitData.GetActor()))
				{
					/*Call the hit event for world actors*/
					OnHitWorld(HitData);
				}
			}
		}
	}
}

FName AWP_BaseWeapon::GetLastHitBone()
{
	if (HitBone != "")
	{
		return HitBone;
	}
	return "";
}

