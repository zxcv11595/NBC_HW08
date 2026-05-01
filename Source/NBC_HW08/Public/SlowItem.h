// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "SlowItem.generated.h"

/**
 * 
 */
UCLASS()
class NBC_HW08_API ASlowItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	ASlowItem();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debuff")
	int SlowDuration;

	virtual void ActivateItem(AActor* Activator) override;
};
