// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseControlItem.h"
#include "SpartaCharacter.h"

AReverseControlItem::AReverseControlItem()
{
	ReverseDuration = 5.0f;
	ItemType = "ReverseControl";
}

void AReverseControlItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
	{
		PlayerCharacter->ApplyReverseControl(ReverseDuration);
		DestroyItem();
	}
}
