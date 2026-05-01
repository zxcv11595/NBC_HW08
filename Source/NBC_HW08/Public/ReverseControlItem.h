// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "ReverseControlItem.generated.h"

/**
 * 
 */
UCLASS()
class NBC_HW08_API AReverseControlItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	AReverseControlItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	int ReverseDuration;

	virtual void ActivateItem(AActor* Activator) override;
};
