#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FindSessionsCallbackProxy.h"
#include "StfSessionLibrary.generated.h"

USTRUCT(BlueprintType)
struct FStfDelegateHandle
{
	GENERATED_BODY()

	FDelegateHandle Handle;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(
	FStfOnSessionUserInviteAcceptedDelegate,
	bool, Success,
	const FBlueprintSessionResult&, InviteResult
);

USTRUCT(BlueprintType)
struct FStfOnlineSessionSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int NumPublicConnections = 100;
};

UCLASS()
class STFONLINESUBSYSTEM_API UStfSessionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(
		BlueprintCallable,
		Category="StfSessionLibrary",
		meta=(Latent, LatentInfo="LatentInfo", WorldContext="WorldContextObject")
	)
	static void CreateSession(
		UObject* WorldContextObject,
		APlayerController* PlayerController,
		FStfOnlineSessionSettings Settings,
		bool& Success,
		FLatentActionInfo LatentInfo
	);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static FStfDelegateHandle AddOnSessionUserInviteAccepted(
		const UObject* WorldContextObject,
		FStfOnSessionUserInviteAcceptedDelegate Event
	);

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void ClearOnSessionUserInviteAccepted(
		const UObject* WorldContextObject,
		FStfDelegateHandle Handle
	);
};
