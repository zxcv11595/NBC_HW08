// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowItem.h"
#include "SpartaCharacter.h"

ASlowItem::ASlowItem()
{
	SlowDuration = 5.0f;
	ItemType = "Slowing";
}

void ASlowItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{ 
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->ApplySlow(SlowDuration);
			DestroyItem();
		}
	}
}
