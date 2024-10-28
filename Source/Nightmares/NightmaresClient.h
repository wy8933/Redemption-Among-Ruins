#pragma once

#include "CoreMinimal.h"

#include "alice/network/connection.h"

#include "NightmaresClient.generated.h"

UENUM(BlueprintType)
enum EConnectState
{
	Connecting,
	Connected,
	Disconnected
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeConnectState, EConnectState, ConnectState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRead, FString, Message);

UCLASS(BlueprintType, Blueprintable)
class NIGHTMARES_API UNightmaresClient : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	asio::io_context IOPrivate;
	TSharedPtr<alice::network::Connection> ClientPrivate;

public:
	UPROPERTY(BlueprintAssignable)
	FOnChangeConnectState OnChangeConnectState;

	UPROPERTY(BlueprintAssignable)
	FOnRead OnRead;

	UFUNCTION(BlueprintCallable)
	void Connect(const FString& Ip, const int Port);

	UFUNCTION(BlueprintCallable)
	void Update();

	UFUNCTION(BlueprintCallable)
	void Send(const FString& Msg);
};
