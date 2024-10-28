#pragma once

#include "CoreMinimal.h"
#include "StfObject.generated.h"

UCLASS(Blueprintable, BlueprintType, meta=(ShowWorldContextPin))
class STFOBJECT_API UStfObject final : public UObject
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;

	UFUNCTION(BlueprintCallable)
	void Replicate();
};